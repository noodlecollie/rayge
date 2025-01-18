#pragma once

#include <stddef.h>
#include <stdint.h>
#include "RayGE/ResourceHandle.h"
#include "Resources/ResourceDomains.h"

typedef struct ResourceList ResourceList;

typedef struct ResourceListAttributes
{
	InternalResourceDomain domain;

	// These are uint32s, to match the range of a resource handle.
	// Max capacity should be cleanly divisible by items per bucket.
	uint32_t maxCapacity;
	uint32_t itemsPerBucket;

	// Memory to allocate per item.
	size_t itemSizeInBytes;

	// Called when an item is about to be destroyed.
	// Do not free the item's memory, but do clean up
	// anything that the item holds that requires it.
	// This callback may be left null if no cleanup
	// needs to take place.
	void (*DeinitItem)(void* item);
} ResourceListAttributes;

typedef enum ResourceListErrorCode
{
	RESOURCELIST_ERROR_NONE = 0,
	RESOURCELIST_ERROR_INVALID_ARGUMENT,
	RESOURCELIST_ERROR_NO_FREE_SPACE,
} ResourceListErrorCode;

ResourceList* ResourceList_Create(ResourceListAttributes attributes);
void ResourceList_Destroy(ResourceList* list);

ResourceListErrorCode
ResourceList_CreateNewItem(ResourceList* list, RayGE_ResourceHandle* outHandle);
void* ResourceList_GetItemData(const ResourceList* list, RayGE_ResourceHandle handle);
