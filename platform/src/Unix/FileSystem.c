#include <sys/types.h>
#include <dirent.h>
#include <stddef.h>
#include <stdlib.h>
#include "Platform/FileSystem.h"
#include "Platform/String.h"
#include "Common/FileSystemCommon.h"

struct Platform_DirectoryListing* Platform_GetDirectoryListing(const char* directory)
{
	DIR* dirPtr = opendir(directory);

	if ( !dirPtr )
	{
		return NULL;
	}

	Platform_DirectoryListing* listing = calloc(1, sizeof(Platform_DirectoryListing));

	if ( !listing )
	{
		return NULL;
	}

	listing->path = Platform_DuplicateString(directory);

	Platform_DirectoryEntry* prevEntry = NULL;

	for ( struct dirent* entry = readdir(dirPtr); entry; entry = readdir(dirPtr) )
	{
		Platform_DirectoryEntry* listingEntry = calloc(1, sizeof(Platform_DirectoryEntry));
		listingEntry->prev = prevEntry;

		if ( !listingEntry->prev )
		{
			// We were setting the head
			listing->entries = listingEntry;
		}
		else
		{
			// Hook up to this entry
			listingEntry->prev->next = listingEntry;
		}

		listingEntry->name = Platform_DuplicateString(entry->d_name);

		switch ( entry->d_type )
		{
			case DT_REG:
			{
				listingEntry->type = PLATFORM_NODE_FILE;
				break;
			}

			case DT_DIR:
			{
				listingEntry->type = PLATFORM_NODE_DIRECTORY;
				break;
			}

			default:
			{
				listingEntry->type = PLATFORM_NODE_UNKNOWN;
				break;
			}
		}

		prevEntry = listingEntry;
	}

	return listing;
}
