#include <stdbool.h>
#include "Resources/ResourceList.h"
#include "MemPool/MemPoolManager.h"
#include "Resources/ResourceHandleUtils.h"
#include "Debugging.h"

typedef struct ResourceItemHeader
{
	bool occupied;
	uint64_t key;
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

static RayGE_ResourceHandle CreateItemInFirstFreeListSlot(ResourceList* list)
{
	size_t bucketIndex = 0;
	ResourceBucket* bucket = GetFirstAvailableBucket(list, &bucketIndex);

	size_t itemIndex = 0;
	ResourceItemHeader* header = GetFirstAvailableHeader(list, bucket, &itemIndex);

	const uint32_t globalIndex = ((uint32_t)bucketIndex * list->atts.itemsPerBucket) + (uint32_t)itemIndex;

	header->key = Resource_CreateKey(globalIndex);
	header->occupied = true;

	++list->totalResources;
	return Resource_CreateInternalHandle(list->atts.domain, globalIndex, header->key);
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

	FreeResourceList(list);
}

ResourceListErrorCode
ResourceList_AddItem(ResourceList* list, RayGE_ResourceHandle* outHandle)
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

	*outHandle = CreateItemInFirstFreeListSlot(list);
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
