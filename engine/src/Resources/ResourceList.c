#include <stdbool.h>
#include "Resources/ResourceList.h"
#include "MemPool/MemPoolManager.h"
#include "Resources/ResourceHandleUtils.h"
#include "Debugging.h"

#define UTHASH_POOLED_MEMPOOL MEMPOOL_RESOURCE_MANAGEMENT
#include "UTUtils/UTHash_Pooled.h"

typedef struct PathToResourceHandleHashItem
{
	UT_hash_handle hh;
	char* path;
	RayGE_ResourceHandle handle;
} PathToResourceHandleHashItem;

typedef struct ResourceItemHeader
{
	bool occupied;
	uint64_t key;
	PathToResourceHandleHashItem* hashItem;
} ResourceItemHeader;

typedef struct ResourceBucket
{
	void* items;
	size_t numItems;

	bool isNotEmpty;
	bool isFull;
} ResourceBucket;

struct ResourceList
{
	ResourceListAttributes atts;

	// This list is sparse - a batch pointer may be null
	// until its slot is used.
	ResourceBucket** buckets;
	size_t numBuckets;

	size_t totalResources;
	PathToResourceHandleHashItem* pathToResourceHandle;
};

static void* GetItemData(ResourceItemHeader* header)
{
	return (void*)((uint8_t*)header + sizeof(ResourceItemHeader));
}

static ResourceItemHeader* GetHeader(const ResourceList* list, ResourceBucket* bucket, size_t index)
{
	return (ResourceItemHeader*)((uint8_t*)bucket->items +
								 (index * (sizeof(ResourceItemHeader) + list->atts.itemSizeInBytes)));
}

static ResourceItemHeader* GetFirstHeader(ResourceBucket* bucket)
{
	return (ResourceItemHeader*)bucket->items;
}

static ResourceItemHeader* GetNextHeader(const ResourceList* list, ResourceItemHeader* header)
{
	return (ResourceItemHeader*)((uint8_t*)header + sizeof(ResourceItemHeader) + list->atts.itemSizeInBytes);
}

static void DeinitItemData(ResourceList* list, ResourceItemHeader* header)
{
	if ( list->atts.DeinitItem && header->occupied )
	{
		list->atts.DeinitItem(GetItemData(header));
	}
}

static void FreeBucket(ResourceList* list, ResourceBucket* bucket)
{
	ResourceItemHeader* header = (ResourceItemHeader*)bucket->items;

	for ( size_t index = 0; index < bucket->numItems; ++index )
	{
		DeinitItemData(list, header);
		header = GetNextHeader(list, header);
	}

	MEMPOOL_FREE(bucket->items);
	MEMPOOL_FREE(bucket);
}

static void FreeResourceList(ResourceList* list)
{
	for ( size_t index = 0; index < list->numBuckets; ++index )
	{
		if ( list->buckets[index] )
		{
			FreeBucket(list, list->buckets[index]);
		}
	}

	MEMPOOL_FREE(list);
}

static ResourceItemHeader*
GetHeaderFromGlobalIndex(const ResourceList* list, uint32_t globalIndex, ResourceBucket** outBucket)
{
	if ( globalIndex >= list->atts.maxCapacity )
	{
		return NULL;
	}

	uint32_t bucketIndex = globalIndex / list->atts.itemsPerBucket;
	ResourceBucket* bucket = list->buckets[bucketIndex];

	if ( outBucket )
	{
		*outBucket = bucket;
	}

	if ( !bucket )
	{
		return NULL;
	}

	uint32_t itemIndex = globalIndex % list->atts.itemsPerBucket;
	return GetHeader(list, bucket, itemIndex);
}

static ResourceList* CreateResourceList(const ResourceListAttributes* attributes)
{
	ResourceList* list = MEMPOOL_CALLOC_STRUCT(MEMPOOL_RESOURCE_MANAGEMENT, ResourceList);

	list->atts = *attributes;
	list->numBuckets = list->atts.maxCapacity / list->atts.itemsPerBucket;
	list->buckets =
		(ResourceBucket**)MEMPOOL_CALLOC(MEMPOOL_RESOURCE_MANAGEMENT, list->numBuckets, sizeof(ResourceBucket*));

	return list;
}

static ResourceBucket* GetFirstAvailableBucket(ResourceList* list, size_t* outIndex)
{
	for ( size_t index = 0; index < list->numBuckets; ++index )
	{
		if ( !list->buckets[index] )
		{
			list->buckets[index] = MEMPOOL_CALLOC_STRUCT(MEMPOOL_RESOURCE_MANAGEMENT, ResourceBucket);
		}

		if ( !list->buckets[index]->isFull )
		{
			if ( outIndex )
			{
				*outIndex = index;
			}

			return list->buckets[index];
		}
	}

	// This function should not have been called if there was no free space.
	RAYGE_ENSURE(false, "Expected to be able to find a resource bucket with a free item!");
	return NULL;
}

static ResourceItemHeader* GetFirstAvailableHeader(ResourceList* list, ResourceBucket* bucket, size_t* outIndex)
{
	ResourceItemHeader* header = (ResourceItemHeader*)bucket->items;

	for ( size_t index = 0; index < bucket->numItems; ++index )
	{
		if ( !header->occupied )
		{
			if ( outIndex )
			{
				*outIndex = index;
			}

			return header;
		}

		header = GetNextHeader(list, header);
	}

	// This function should not have been called if there was no free space.
	RAYGE_ENSURE(false, "Expected to be able to find a free resource item within the bucket!");
	return NULL;
}

static RayGE_ResourceHandle CreateItemInFirstFreeListSlot(ResourceList* list, const char* path)
{
	size_t bucketIndex = 0;
	ResourceBucket* bucket = GetFirstAvailableBucket(list, &bucketIndex);

	size_t itemIndex = 0;
	ResourceItemHeader* header = GetFirstAvailableHeader(list, bucket, &itemIndex);

	PathToResourceHandleHashItem* hashItem = MEMPOOL_CALLOC_STRUCT(MEMPOOL_RESOURCE_MANAGEMENT, PathToResourceHandleHashItem);
	HASH_ADD_STR(list->pathToResourceHandle, path, hashItem);

	const uint32_t globalIndex = ((uint32_t)bucketIndex * list->atts.itemsPerBucket) + (uint32_t)itemIndex;

	header->occupied = true;
	header->key = Resource_CreateKey(globalIndex);
	header->hashItem = hashItem;

	hashItem->path = wzl_strdup(path);
	hashItem->handle = Resource_CreateInternalHandle(list->atts.domain, globalIndex, header->key);

	void* itemData = GetItemData(header);
	memset(itemData, 0, list->atts.itemSizeInBytes);

	++list->totalResources;
	return hashItem->handle;
}

static void DeleteHashEntry(ResourceList* list, PathToResourceHandleHashItem* item)
{
	HASH_DEL(list->pathToResourceHandle, item);
	MEMPOOL_FREE(item->path);
	MEMPOOL_FREE(item);
}

static void DeleteAllHashEntries(ResourceList* list)
{
	PathToResourceHandleHashItem* item = NULL;
	PathToResourceHandleHashItem* temp = NULL;

	HASH_ITER(hh, list->pathToResourceHandle, item, temp)
	{
		DeleteHashEntry(list, item);
	}
}

static RayGE_ResourceHandle FindResourceHandleByPath(ResourceList* list, const char* path)
{
	PathToResourceHandleHashItem* item = NULL;
	HASH_FIND_STR(list->pathToResourceHandle, path, item);

	return item ? item->handle : RAYGE_NULL_RESOURCE_HANDLE;
}

static ResourceListIterator CreateInvalidIterator(ResourceList* list)
{
	return (ResourceListIterator) {
		.list = list,
		.globalIndex = list ? list->atts.maxCapacity : 0,
	};
}

static bool IteratorIsInRangeOfList(const ResourceListIterator* iterator)
{
	return iterator && iterator->list && iterator->globalIndex < iterator->list->atts.maxCapacity;
}

// Assumes iterator index is in range.
static bool IncrementIteratorToNextValidItem(ResourceListIterator* iterator)
{
	ResourceItemHeader* header = GetHeaderFromGlobalIndex(iterator->list, iterator->globalIndex, NULL);
	RAYGE_ASSERT_VALID(header);

	if ( !header )
	{
		return false;
	}

	uint32_t itemIndex = iterator->globalIndex % iterator->list->atts.itemsPerBucket;

	while ( true )
	{
		// For checking overflow later.
		const uint32_t lastGlobalIndex = iterator->globalIndex;

		// Move on by one item.
		++iterator->globalIndex;
		++itemIndex;
		header = GetNextHeader(iterator->list, header);

		if ( lastGlobalIndex > iterator->globalIndex || itemIndex >= iterator->list->atts.itemsPerBucket )
		{
			// Ran out of items.
			break;
		}

		if ( header->occupied )
		{
			// This item is valid.
			return true;
		}
	}

	return false;
}

// Assumes iterator index is in range.
static bool IncrementIteratorToNextValidBucket(ResourceListIterator* iterator, ResourceBucket** outBucket)
{
	if ( outBucket )
	{
		*outBucket = NULL;
	}

	ResourceBucket* bucket = NULL;
	GetHeaderFromGlobalIndex(iterator->list, iterator->globalIndex, &bucket);
	RAYGE_ASSERT_VALID(bucket);

	if ( !bucket )
	{
		return false;
	}

	// Reset to the first index in the bucket, since we're about to move on
	// and want to land at the beginning of the next bucket.
	const uint32_t itemsPerBucket = iterator->list->atts.itemsPerBucket;
	iterator->globalIndex = (iterator->globalIndex / itemsPerBucket) * itemsPerBucket;

	// Sanity:
	RAYGE_ASSERT(
		iterator->list->buckets[iterator->globalIndex / itemsPerBucket] == bucket,
		"Unexpected change in bucket after modifying iterator global index!"
	);

	uint32_t bucketIndex = iterator->globalIndex / itemsPerBucket;

	while ( true )
	{
		// For checking overflow later.
		const uint32_t lastGlobalIndex = iterator->globalIndex;

		// Move on by one bucket.
		iterator->globalIndex += itemsPerBucket;
		++bucketIndex;

		if ( lastGlobalIndex < iterator->globalIndex || bucketIndex >= iterator->list->numBuckets )
		{
			// Ran out of buckets.
			break;
		}

		bucket = iterator->list->buckets[bucketIndex];

		if ( bucket && bucket->isNotEmpty )
		{
			// This bucket is valid.

			if ( outBucket )
			{
				*outBucket = bucket;
			}

			return true;
		}
	}

	return false;
}

ResourceList* ResourceList_Create(ResourceListAttributes attributes)
{
	RAYGE_ASSERT(
		attributes.domain > RESOURCE_DOMAIN_INVALID && attributes.domain < RESOURCE_DOMAIN__COUNT,
		"Domain must be valid"
	);

	RAYGE_ASSERT(attributes.maxCapacity > 0, "Capacity must be greater than zero");
	RAYGE_ASSERT(attributes.itemsPerBucket > 0, "Items per bucket must be greater than zero");

	if ( attributes.domain == RESOURCE_DOMAIN_INVALID || attributes.domain >= RESOURCE_DOMAIN__COUNT ||
		 attributes.maxCapacity < 1 || attributes.itemsPerBucket < 1 )
	{
		return NULL;
	}

	uint32_t remainder = attributes.maxCapacity % attributes.itemsPerBucket;

	if ( remainder > 0 )
	{
		Logging_PrintLine(
			RAYGE_LOG_WARNING,
			"ResourceList_Create: Items per bucket (%zu) did not neatly divide capacity (%zu), leaving a remainder of "
			"%zu. New capacity of list will be %zu items.",
			attributes.itemsPerBucket,
			attributes.maxCapacity,
			remainder,
			attributes.maxCapacity - remainder
		);

		attributes.maxCapacity -= remainder;

		if ( attributes.maxCapacity < 1 )
		{
			RAYGE_ASSERT(false, "Removing remainder created an empty list");
			return NULL;
		}
	}

	return CreateResourceList(&attributes);
}

void ResourceList_Destroy(ResourceList* list)
{
	RAYGE_ASSERT_VALID(list);

	if ( !list )
	{
		return;
	}

	DeleteAllHashEntries(list);
	FreeResourceList(list);
}

ResourceListErrorCode ResourceList_AddItem(ResourceList* list, const char* path, RayGE_ResourceHandle* outHandle)
{
	RAYGE_ASSERT_VALID(outHandle);

	if ( !outHandle )
	{
		return RESOURCELIST_ERROR_INVALID_ARGUMENT;
	}

	*outHandle = RAYGE_NULL_RESOURCE_HANDLE;

	RAYGE_ASSERT_VALID(list);

	if ( !list )
	{
		return RESOURCELIST_ERROR_INVALID_ARGUMENT;
	}

	if ( list->totalResources >= list->atts.maxCapacity )
	{
		return RESOURCELIST_ERROR_NO_FREE_SPACE;
	}

	if ( path )
	{
		RayGE_ResourceHandle handle = FindResourceHandleByPath(list, path);

		if ( !RAYGE_IS_NULL_RESOURCE_HANDLE(handle) )
		{
			*outHandle = handle;
			return RESOURCELIST_ERROR_NONE;
		}
	}

	*outHandle = CreateItemInFirstFreeListSlot(list, path);
	return RESOURCELIST_ERROR_NONE;
}

void* ResourceList_GetItemData(const ResourceList* list, RayGE_ResourceHandle handle)
{
	RAYGE_ASSERT_VALID(list);

	if ( !list )
	{
		return NULL;
	}

	if ( !Resource_HandleIsValidForInternalDomain(handle, list->atts.domain, list->atts.maxCapacity) )
	{
		return NULL;
	}

	ResourceItemHeader* header = GetHeaderFromGlobalIndex((ResourceList*)list, handle.index, NULL);

	if ( !header || !header->occupied || header->key != handle.key )
	{
		return NULL;
	}

	return GetItemData(header);
}

ResourceListIterator ResourceList_GetIteratorToFirstItem(ResourceList* list)
{
	RAYGE_ASSERT_VALID(list);

	if ( !list )
	{
		return CreateInvalidIterator(NULL);
	}

	ResourceListIterator iterator = {
		.list = list,
		.globalIndex = 0,
	};

	if ( !ResourceList_IteratorIsValid(iterator) )
	{
		iterator = ResourceList_IncrementIterator(iterator);
	}

	return iterator;
}

ResourceListIterator ResourceList_IncrementIterator(ResourceListIterator iterator)
{
	if ( !IteratorIsInRangeOfList(&iterator) )
	{
		return CreateInvalidIterator(iterator.list);
	}

	while ( true )
	{
		if ( IncrementIteratorToNextValidItem(&iterator) )
		{
			// We found another item in the same bucket, so we're fine.
			return iterator;
		}

		// If we get here, we ran out of items in the bucket, so move to the next bucket.
		ResourceBucket* bucket = NULL;

		if ( !IncrementIteratorToNextValidBucket(&iterator, &bucket) )
		{
			// No more buckets, so we're at the end of the list.
			break;
		}

		RAYGE_ASSERT_VALID(bucket);

		// Check that the first items in the bucket is valid.
		// If it is, we can stop. If not, we go around the loop again,
		// which will find the next valid item.
		if ( GetFirstHeader(bucket)->occupied )
		{
			return iterator;
		}
	}

	return CreateInvalidIterator(iterator.list);
}

bool ResourceList_IteratorIsValid(ResourceListIterator iterator)
{
	if ( !IteratorIsInRangeOfList(&iterator) )
	{
		return false;
	}

	ResourceItemHeader* header = GetHeaderFromGlobalIndex(iterator.list, iterator.globalIndex, NULL);
	return header && header->occupied;
}

void* ResourceList_GetItemDataFromIterator(ResourceListIterator iterator)
{
	if ( !IteratorIsInRangeOfList(&iterator) )
	{
		return false;
	}

	ResourceItemHeader* header = GetHeaderFromGlobalIndex(iterator.list, iterator.globalIndex, NULL);
	return (header && header->occupied) ? GetItemData(header) : NULL;
}
