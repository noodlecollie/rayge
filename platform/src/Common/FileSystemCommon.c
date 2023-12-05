#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Platform/FileSystemNative.h"
#include "Common/FileSystemCommon.h"
#include "wzl_cutl/filesystem.h"
#include "wzl_cutl/string.h"
#include "cwalk.h"

static Platform_Path g_ExecutableDirectory;

bool Platform_DirectoryExists(const char* path)
{
	if ( !path || !(*path) )
	{
		return false;
	}

	char* nativePath = Platform_NativeAbsolutePathFromExecutableDirectory(path);
	const bool exists = wzl_directory_exists(nativePath);
	free(nativePath);

	return exists;
}

bool Platform_FileExists(const char* path)
{
	if ( !path || !(*path) )
	{
		return false;
	}

	char* nativePath = Platform_NativeAbsolutePathFromExecutableDirectory(path);
	const bool exists = wzl_file_exists(nativePath);
	free(nativePath);

	return exists;
}

bool Platform_PathIsAbsolute(const char* path)
{
	return path && path[0] == '/';
}

bool Platform_SetExecutableFromArgV0(const char* nativePath)
{
	if ( !nativePath )
	{
		return false;
	}

	Platform_Path temp;

	if ( cwk_path_is_relative(nativePath) )
	{
		char* cwdBuffer = wzl_get_cwd(NULL, 0);

		if ( !cwdBuffer )
		{
			return false;
		}

		cwk_path_get_absolute(cwdBuffer, nativePath, temp, sizeof(temp));
		free(cwdBuffer);

		nativePath = temp;
	}

	size_t length = 0;
	cwk_path_get_dirname(nativePath, &length);

	if ( length < 1 || length >= sizeof(g_ExecutableDirectory) )
	{
		return false;
	}

	if ( nativePath[length - 1] == '/' )
	{
		--length;
	}

	memcpy(g_ExecutableDirectory, nativePath, length);
	g_ExecutableDirectory[length + 1] = '\0';

	return true;
}

char* Platform_NativeAbsolutePathFromExecutableDirectory(const char* relativePath)
{
	char* nativeRelPath = wzl_duplicate_string(relativePath ? relativePath : "");
	Platform_PathSeparatorsToNative(nativeRelPath);

	char* out = NULL;

	char dummy;
	const size_t requiredSize = cwk_path_get_absolute(g_ExecutableDirectory, nativeRelPath, &dummy, sizeof(dummy)) + 1;

	// Safety:
	if ( requiredSize > 1 )
	{
		out = malloc(requiredSize);
		cwk_path_get_absolute(g_ExecutableDirectory, nativeRelPath, out, requiredSize);
	}
	else
	{
		out = wzl_duplicate_string("");
	}

	free(nativeRelPath);
	return out;
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
