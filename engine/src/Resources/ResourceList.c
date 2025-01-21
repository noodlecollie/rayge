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

static size_t GetFullItemSize(const ResourceList* list)
{
	return sizeof(ResourceItemHeader) + list->atts.itemSizeInBytes;
}

static void* GetItemData(ResourceItemHeader* header)
{
	return (void*)((uint8_t*)header + sizeof(ResourceItemHeader));
}

static ResourceItemHeader* GetHeader(const ResourceList* list, ResourceBucket* bucket, size_t index)
{
	return (ResourceItemHeader*)((uint8_t*)bucket->items + (index * GetFullItemSize(list)));
}

static ResourceItemHeader* GetFirstHeader(ResourceBucket* bucket)
{
	return (ResourceItemHeader*)bucket->items;
}

static ResourceItemHeader* GetNextHeader(const ResourceList* list, ResourceItemHeader* header)
{
	return (ResourceItemHeader*)((uint8_t*)header + GetFullItemSize(list));
}

static void DeleteHashEntry(ResourceList* list, PathToResourceHandleHashItem* item)
{
	if ( !item )
	{
		return;
	}

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

static void DeinitItemData(ResourceList* list, ResourceItemHeader* header)
{
	if ( list->atts.DeinitItem && header->occupied )
	{
		list->atts.DeinitItem(GetItemData(header));
	}
}

static void FreeBucket(ResourceList* list, ResourceBucket* bucket)
{
	if ( bucket->isNotEmpty )
	{
		ResourceItemHeader* header = (ResourceItemHeader*)bucket->items;

		for ( size_t index = 0; index < bucket->numItems; ++index )
		{
			DeinitItemData(list, header);
			header = GetNextHeader(list, header);
		}
	}

	MEMPOOL_FREE(bucket->items);
	MEMPOOL_FREE(bucket);
}

static void DestroyBucketIfEmpty(ResourceList* list, ResourceBucket* bucket)
{
	if ( bucket->isNotEmpty )
	{
		return;
	}

	for ( size_t index = 0; index < list->numBuckets; ++index )
	{
		if ( list->buckets[index] == bucket )
		{
			FreeBucket(list, bucket);
			list->buckets[index] = NULL;
			break;
		}
	}
}

static void FreeResourceList(ResourceList* list)
{
	DeleteAllHashEntries(list);

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
	if ( outBucket )
	{
		*outBucket = NULL;
	}

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

static ResourceBucket* CreateBucket(ResourceList* list)
{
	ResourceBucket* bucket = MEMPOOL_CALLOC_STRUCT(MEMPOOL_RESOURCE_MANAGEMENT, ResourceBucket);
	bucket->numItems = list->atts.itemsPerBucket;
	bucket->items = MEMPOOL_CALLOC(MEMPOOL_RESOURCE_MANAGEMENT, bucket->numItems, GetFullItemSize(list));
	return bucket;
}

static ResourceBucket* GetFirstAvailableBucket(ResourceList* list, size_t* outIndex)
{
	for ( size_t index = 0; index < list->numBuckets; ++index )
	{
		if ( !list->buckets[index] )
		{
			list->buckets[index] = CreateBucket(list);
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

static ResourceItemHeader*
GetHeaderFromHandle(const ResourceList* list, RayGE_ResourceHandle handle, ResourceBucket** outBucket)
{
	if ( outBucket )
	{
		*outBucket = NULL;
	}

	if ( !Resource_HandleIsValidForInternalDomain(handle, list->atts.domain, list->atts.maxCapacity) )
	{
		return NULL;
	}

	ResourceBucket* bucket = NULL;
	ResourceItemHeader* header = GetHeaderFromGlobalIndex(list, handle.index, &bucket);

	if ( !header || !header->occupied || header->key != handle.key )
	{
		return NULL;
	}

	if ( outBucket )
	{
		*outBucket = bucket;
	}

	return header;
}

static void UpdateBucketFlags(ResourceList* list, ResourceBucket* bucket)
{
	bucket->isFull = true;
	bucket->isNotEmpty = false;

	ResourceItemHeader* header = GetFirstHeader(bucket);

	for ( size_t index = 0; index < bucket->numItems; ++index )
	{
		if ( !header->occupied )
		{
			bucket->isFull = false;
		}
		else
		{
			bucket->isNotEmpty = true;
		}

		if ( !bucket->isFull && bucket->isNotEmpty )
		{
			// We can quit now - there will be no more changes to any flags.
			return;
		}

		header = GetNextHeader(list, header);
	}
}

static RayGE_ResourceHandle CreateItemInFirstFreeListSlot(ResourceList* list, const char* path)
{
	size_t bucketIndex = 0;
	ResourceBucket* bucket = GetFirstAvailableBucket(list, &bucketIndex);

	size_t itemIndex = 0;
	ResourceItemHeader* header = GetFirstAvailableHeader(list, bucket, &itemIndex);

	PathToResourceHandleHashItem* hashItem =
		path ? MEMPOOL_CALLOC_STRUCT(MEMPOOL_RESOURCE_MANAGEMENT, PathToResourceHandleHashItem) : NULL;

	const uint32_t globalIndex = ((uint32_t)bucketIndex * list->atts.itemsPerBucket) + (uint32_t)itemIndex;
	const uint64_t key = Resource_CreateKey(globalIndex);
	RayGE_ResourceHandle handle = Resource_CreateInternalHandle(list->atts.domain, globalIndex, key);

	header->occupied = true;
	header->key = key;
	header->hashItem = hashItem;

	if ( hashItem )
	{
		hashItem->path = wzl_strdup(path);
		hashItem->handle = handle;

		HASH_ADD_STR(list->pathToResourceHandle, path, hashItem);
	}

	void* itemData = GetItemData(header);
	memset(itemData, 0, list->atts.itemSizeInBytes);

	UpdateBucketFlags(list, bucket);

	++list->totalResources;
	return handle;
}

static void DestroyItem(ResourceList* list, ResourceItemHeader* header)
{
	DeleteHashEntry(list, header->hashItem);
	DeinitItemData(list, header);

	header->key = 0;
	header->occupied = false;

	--list->totalResources;
}

static RayGE_ResourceHandle FindResourceHandleByPath(ResourceList* list, const char* path)
{
	RAYGE_ASSERT_VALID(path);

	PathToResourceHandleHashItem* item = NULL;
	HASH_FIND_STR(list->pathToResourceHandle, path, item);

	return item ? item->handle : RAYGE_NULL_RESOURCE_HANDLE;
}

static ResourceListIterator CreateInvalidIterator(const ResourceList* list)
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
	RAYGE_ASSERT(
		attributes.itemsPerBucket <= attributes.maxCapacity,
		"Capacity must not be smaller than items per bucket"
	);
	RAYGE_ASSERT(attributes.itemSizeInBytes > 0, "Item size must be greater than zero");

	if ( attributes.domain == RESOURCE_DOMAIN_INVALID || attributes.domain >= RESOURCE_DOMAIN__COUNT ||
		 attributes.maxCapacity < 1 || attributes.itemsPerBucket < 1 ||
		 attributes.itemsPerBucket > attributes.maxCapacity || attributes.itemSizeInBytes < 1 )
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

	FreeResourceList(list);
}

size_t ResourceList_ItemCount(const ResourceList* list)
{
	RAYGE_ASSERT_VALID(list);
	return list ? list->totalResources : 0;
}

ResourceListErrorCode ResourceList_CreateNewItem(ResourceList* list, const char* path, RayGE_ResourceHandle* outHandle)
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

void ResourceList_DestroyItem(ResourceList* list, RayGE_ResourceHandle handle)
{
	RAYGE_ASSERT_VALID(list);

	if ( !list )
	{
		return;
	}

	ResourceBucket* bucket = NULL;
	ResourceItemHeader* header = GetHeaderFromHandle(list, handle, &bucket);

	if ( !header )
	{
		return;
	}

	DestroyItem(list, header);
	UpdateBucketFlags(list, bucket);
	DestroyBucketIfEmpty(list, bucket);
}

void* ResourceList_GetItemData(const ResourceList* list, RayGE_ResourceHandle handle)
{
	RAYGE_ASSERT_VALID(list);

	if ( !list )
	{
		return NULL;
	}

	ResourceItemHeader* header = GetHeaderFromHandle(list, handle, NULL);
	return header ? GetItemData(header) : NULL;
}

const char* ResourceList_GetItemPath(const ResourceList* list, RayGE_ResourceHandle handle)
{
	RAYGE_ASSERT_VALID(list);

	if ( !list )
	{
		return NULL;
	}

	ResourceItemHeader* header = GetHeaderFromHandle(list, handle, NULL);
	return header->hashItem ? header->hashItem->path : NULL;
}

ResourceListIterator ResourceList_GetIteratorToFirstItem(const ResourceList* list)
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
		return NULL;
	}

	ResourceItemHeader* header = GetHeaderFromGlobalIndex(iterator.list, iterator.globalIndex, NULL);
	return (header && header->occupied) ? GetItemData(header) : NULL;
}

const char* ResourceList_GetItemPathFromIterator(ResourceListIterator iterator)
{
	if ( !IteratorIsInRangeOfList(&iterator) )
	{
		return NULL;
	}

	ResourceItemHeader* header = GetHeaderFromGlobalIndex(iterator.list, iterator.globalIndex, NULL);
	return (header && header->occupied && header->hashItem) ? header->hashItem->path : NULL;
}

#if RAYGE_BUILD_TESTING()
static void TestInvalidArguments(void)
{
	ResourceListAttributes atts;

	// Invalid domain
	atts = (ResourceListAttributes) {
		.domain = RESOURCE_DOMAIN_INVALID,
		.itemSizeInBytes = 4,
		.itemsPerBucket = 4,
		.maxCapacity = 8,
	};

	// Zero item size
	atts = (ResourceListAttributes) {
		.domain = RESOURCE_DOMAIN_ENTITY,
		.itemSizeInBytes = 0,
		.itemsPerBucket = 4,
		.maxCapacity = 8,
	};

	TESTING_CHECK(!ResourceList_Create(atts));

	// Zero capacity
	atts = (ResourceListAttributes) {
		.domain = RESOURCE_DOMAIN_ENTITY,
		.itemSizeInBytes = 4,
		.itemsPerBucket = 4,
		.maxCapacity = 0,
	};

	TESTING_CHECK(!ResourceList_Create(atts));

	// Zero items per bucket
	atts = (ResourceListAttributes) {
		.domain = RESOURCE_DOMAIN_ENTITY,
		.itemSizeInBytes = 4,
		.itemsPerBucket = 0,
		.maxCapacity = 8,
	};

	TESTING_CHECK(!ResourceList_Create(atts));

	// Zero capacity because items per bucket exceeded it
	atts = (ResourceListAttributes) {
		.domain = RESOURCE_DOMAIN_ENTITY,
		.itemSizeInBytes = 4,
		.itemsPerBucket = 9,
		.maxCapacity = 8,
	};

	TESTING_CHECK(!ResourceList_Create(atts));
}

void TestAddingAndRemovingElements(void)
{
	ResourceListAttributes atts = (ResourceListAttributes) {
		.domain = RESOURCE_DOMAIN_ENTITY,
		.itemSizeInBytes = sizeof(int),
		.itemsPerBucket = 4,
		.maxCapacity = 8,
	};

	ResourceList* list = ResourceList_Create(atts);

	if ( !TESTING_CHECK(list) )
	{
		return;
	}

	{
		RayGE_ResourceHandle handle = RAYGE_NULL_RESOURCE_HANDLE;
		ResourceListErrorCode itemCreateResult = RESOURCELIST_ERROR_INVALID_ARGUMENT;
		void* data = NULL;
		int dataValue = 0;

		itemCreateResult = ResourceList_CreateNewItem(list, NULL, &handle);
		data = ResourceList_GetItemData(list, handle);

		if ( data )
		{
			*((int*)data) = 1234;

			void* secondData = ResourceList_GetItemData(list, handle);
			dataValue = *((int*)secondData);
		}

		if ( itemCreateResult == RESOURCELIST_ERROR_NONE )
		{
			ResourceList_DestroyItem(list, handle);
		}

		TESTING_CHECK(!RAYGE_IS_NULL_RESOURCE_HANDLE(handle));
		TESTING_CHECK(itemCreateResult == RESOURCELIST_ERROR_NONE);
		TESTING_CHECK(data);
		TESTING_CHECK(dataValue == 1234);
	}

	// Do the same again, but with a path this time.
	{
		RayGE_ResourceHandle handle = RAYGE_NULL_RESOURCE_HANDLE;
		ResourceListErrorCode itemCreateResult = RESOURCELIST_ERROR_INVALID_ARGUMENT;
		void* data = NULL;
		int dataValue = 0;
		const char* path = NULL;

		itemCreateResult = ResourceList_CreateNewItem(list, "my/item", &handle);
		data = ResourceList_GetItemData(list, handle);

		if ( data )
		{
			*((int*)data) = 5678;

			void* secondData = ResourceList_GetItemData(list, handle);
			dataValue = *((int*)secondData);
		}

		path = ResourceList_GetItemPath(list, handle);

		// No cleanup this time - it should happen in ResourceList_Destroy().

		TESTING_CHECK(!RAYGE_IS_NULL_RESOURCE_HANDLE(handle));
		TESTING_CHECK(itemCreateResult == RESOURCELIST_ERROR_NONE);
		TESTING_CHECK(data);
		TESTING_CHECK(dataValue == 5678);
		TESTING_CHECK(wzl_strequals(path, "my/item"));
	}

	ResourceList_Destroy(list);
}

void ResourceList_RunTests(void)
{
	TestInvalidArguments();
	TestAddingAndRemovingElements();
}
#endif
