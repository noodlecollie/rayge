#include <sys/types.h>
#include <dirent.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "Platform/FileSystem.h"
#include "Platform/FileSystemNative.h"
#include "Platform/String.h"
#include "Common/FileSystemCommon.h"
#include "cwalk.h"

static DIR* OpenDir(const char* relativePath)
{
	char* nativeDirectory = Platform_NativeAbsolutePathFromExecutableDirectory(relativePath);

	if ( !nativeDirectory )
	{
		return NULL;
	}

	DIR* dirPtr = opendir(nativeDirectory);
	free(nativeDirectory);
	return dirPtr;
}

void Platform_PathSeparatorsToNative(char* path)
{
	if ( !path )
	{
		return;
	}

	for ( ; *path; ++path )
	{
		if ( *path == '\\' )
		{
			*path = '/';
		}
	}
}

struct Platform_DirectoryListing* Platform_GetDirectoryListing(const char* path)
{
	DIR* dirPtr = OpenDir(path);

	if ( !dirPtr )
	{
		return NULL;
	}

	Platform_DirectoryListing* listing = calloc(1, sizeof(Platform_DirectoryListing));

	if ( !listing )
	{
		closedir(dirPtr);
		return NULL;
	}

	listing->path = Platform_DuplicateString(path);

	Platform_DirectoryEntry* prevEntry = NULL;

	for ( struct dirent* entry = readdir(dirPtr); entry; entry = readdir(dirPtr) )
	{
		if ( strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 )
		{
			continue;
		}

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

	closedir(dirPtr);
	return listing;
}
