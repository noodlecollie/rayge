#include <stdlib.h>
#include <stdio.h>
#include "Platform/FileSystem.h"
#include "Platform/FileSystemNative.h"
#include "Common/FileSystemCommon.h"
#include "wzl_cutl/string.h"
#include "wzl_cutl/filesystem.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void Platform_PathSeparatorsToNative(char* path)
{
	if ( !path )
	{
		return;
	}

	for ( ; *path; ++path )
	{
		if ( *path == '/' )
		{
			*path = '\\';
		}
	}
}

struct Platform_DirectoryListing* Platform_GetDirectoryListing(const char* path)
{
	Platform_Path sPath;
	sprintf_s(sPath, sizeof(sPath), "%s/*.*", path);

	char* nativePath = Platform_NativeAbsolutePathFromExecutableDirectory(path);

	WIN32_FIND_DATA fdFile;
	HANDLE hFind = FindFirstFileA(nativePath, &fdFile);

	free(nativePath);

	if ( hFind == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}

	Platform_DirectoryListing* listing = calloc(1, sizeof(Platform_DirectoryListing));

	if ( !listing )
	{
		FindClose(hFind);
		return NULL;
	}

	listing->path = wzl_duplicate_string(path);

	Platform_DirectoryEntry* prevEntry = NULL;

	do
	{
		if ( strcmp(fdFile.cFileName, ".") == 0 || strcmp(fdFile.cFileName, "..") == 0 )
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

		listingEntry->name = wzl_duplicate_string(fdFile.cFileName);

		if ( fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			listingEntry->type = PLATFORM_NODE_DIRECTORY;
		}
		else
		{
			listingEntry->type = PLATFORM_NODE_FILE;
		}

		prevEntry = listingEntry;
	}
	while ( FindNextFileA(hFind, &fdFile) );

	FindClose(hFind);
	return listing;
}
