#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "RayGE/ResourceHandle.h"
#include "Resources/ResourceDomains.h"
#include "Testing/Testing.h"

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

typedef struct ResourceListIterator
{
	const ResourceList* list;
	uint32_t globalIndex;
} ResourceListIterator;

typedef enum ResourceListErrorCode
{
	RESOURCELIST_ERROR_NONE = 0,
	RESOURCELIST_ERROR_INVALID_ARGUMENT,
	RESOURCELIST_ERROR_NO_FREE_SPACE,
	RESOURCELIST_ERROR_PATH_ALREADY_EXISTED,
} ResourceListErrorCode;

ResourceList* ResourceList_Create(ResourceListAttributes attributes);
void ResourceList_Destroy(ResourceList* list);
size_t ResourceList_ItemCount(const ResourceList* list);

// If a path is provided, and a resource with this path has already been
// added, the existing resource will be returned.
// When an item is successfully created, its data starts out zeroed,
// so there is no need for the caller to do this.
ResourceListErrorCode
ResourceList_CreateNewItem(ResourceList* list, const char* path, RayGE_ResourceHandle* outHandle);

bool ResourceList_DestroyItem(ResourceList* list, RayGE_ResourceHandle handle);
void* ResourceList_GetItemData(const ResourceList* list, RayGE_ResourceHandle handle);
const char* ResourceList_GetItemPath(const ResourceList* list, RayGE_ResourceHandle handle);

ResourceListIterator ResourceList_GetIteratorToFirstItem(const ResourceList* list);
ResourceListIterator ResourceList_GetIteratorFromHandle(const ResourceList* list, RayGE_ResourceHandle handle);
ResourceListIterator ResourceList_IncrementIterator(ResourceListIterator iterator);
bool ResourceList_IteratorIsValid(ResourceListIterator iterator);
void* ResourceList_GetItemDataFromIterator(ResourceListIterator iterator);
const char* ResourceList_GetItemPathFromIterator(ResourceListIterator iterator);

#if RAYGE_BUILD_TESTING()
void ResourceList_RunTests(void);
#endif
