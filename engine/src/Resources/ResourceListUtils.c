#include "Resources/ResourceListUtils.h"
#include "MemPool/MemPoolManager.h"
#include "Utils/StringUtils.h"
#include "Debugging.h"

RayGE_ResourceHandle ResourceListUtils_CreateNewItem(
	const char* itemType,
	ResourceList* list,
	const char* relPath,
	ResourceListUtils_CreationCallback callback,
	void* userData
)
{
	RAYGE_ASSERT_VALID(itemType);
	RAYGE_ASSERT_VALID(list);
	RAYGE_ASSERT(relPath && *relPath, "Path was not valid");
	RAYGE_ASSERT_VALID(callback);

	if ( !itemType || !list || !relPath || !(*relPath) || !callback )
	{
		return RAYGE_NULL_RESOURCE_HANDLE;
	}

	char* trimmedPath = StringUtils_TrimString(MEMPOOL_RESOURCE_MANAGEMENT, relPath);
	RayGE_ResourceHandle outHandle = RAYGE_NULL_RESOURCE_HANDLE;

	do
	{
		ResourceListErrorCode result = ResourceList_CreateNewItem(list, trimmedPath, &outHandle);

		if ( result == RESOURCELIST_ERROR_PATH_ALREADY_EXISTED )
		{
			// Handle will refer to item that existed,
			// so we can just return it.
			break;
		}

		if ( result != RESOURCELIST_ERROR_NONE )
		{
			if ( result == RESOURCELIST_ERROR_NO_FREE_SPACE )
			{
				Logging_PrintLine(
					RAYGE_LOG_ERROR,
					"Cannot load %s %s: reached maximum of %zu instances",
					itemType,
					trimmedPath,
					ResourceList_Capacity(list)
				);
			}
			else
			{
				Logging_PrintLine(
					RAYGE_LOG_ERROR,
					"Failed to %s world %s: could not create resource list item (error code: %d)",
					itemType,
					trimmedPath,
					result
				);
			}

			break;
		}

		void* itemData = ResourceList_GetItemData(list, outHandle);
		RAYGE_ASSERT_VALID(itemData);

		if ( !callback(trimmedPath, itemData, userData) )
		{
			Logging_PrintLine(RAYGE_LOG_ERROR, "Failed to load %s %s", itemType, trimmedPath);
			ResourceList_DestroyItem(list, outHandle);
			outHandle = RAYGE_NULL_RESOURCE_HANDLE;
			break;
		}
	}
	while ( false );

	if ( trimmedPath )
	{
		MEMPOOL_FREE(trimmedPath);
	}

	return outHandle;
}
