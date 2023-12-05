#include <stdlib.h>
#include <stdbool.h>
#include "Common/FileSystemCommon.h"

bool PathBeginsWithWindowsDrive(char* path)
{
	return path && path[0] >= 'A' && path[0] <= 'Z' && path[1] == ':' && path[2] == '\\';
}

void NormalisePathSeparators(char* path)
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

bool Platform_PathIsAbsolute(const char* path)
{
	return path && path[0] == '/';
}

void Platform_FreeDirectoryListing(struct Platform_DirectoryListing* listing)
{
	Platform_DirectoryEntry* entry = listing->entries;

	while ( entry )
	{
		if ( entry->name )
		{
			free(entry->name);
		}

		Platform_DirectoryEntry* next = entry->next;
		free(entry);
		entry = next;
	}

	if ( listing->path )
	{
		free(listing->path);
	}

	free(listing);
}

const char* Platform_DirectoryListing_GetDirectoryPath(struct Platform_DirectoryListing* listing)
{
	return listing ? listing->path : NULL;
}

struct Platform_DirectoryEntry* Platform_DirectoryListing_GetFirstEntry(struct Platform_DirectoryListing* listing)
{
	return listing ? listing->entries : NULL;
}

struct Platform_DirectoryEntry* Platform_DirectoryListing_GetPrevEntry(struct Platform_DirectoryEntry* entry)
{
	return entry ? entry->prev : NULL;
}

struct Platform_DirectoryEntry* Platform_DirectoryListing_GetNextEntry(struct Platform_DirectoryEntry* entry)
{
	return entry ? entry->next : NULL;
}

Platform_FileNodeType Platform_DirectoryListing_GetNodeType(struct Platform_DirectoryEntry* entry)
{
	return entry ? entry->type : PLATFORM_NODE_UNKNOWN;
}

const char* Platform_DirectoryListing_GetNodeName(struct Platform_DirectoryEntry* entry)
{
	return entry ? entry->name : NULL;
}
