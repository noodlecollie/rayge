#include <stdlib.h>
#include <stdio.h>
#include "Platform/FileSystem.h"
#include "Platform/String.h"
#include "Common/FileSystemCommon.h"
#include "cwalk.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <direct.h>

static char g_ExecutableDirectory[MAX_PATH];
static size_t g_ExecutableDirectoryLength = 0;

bool Platform_SetExecutableFromArgV0(const char* path)
{
	if ( !path )
	{
		return false;
	}

	char temp[MAX_PATH];

	if ( cwk_path_is_relative(path) )
	{
		char cwd[MAX_PATH];

		if ( !_getcwd(cwd, sizeof(cwd)) )
		{
			return false;
		}

		cwk_path_get_absolute(cwd, path, temp, sizeof(temp));
		path = temp;
	}

	size_t length = 0;
	cwk_path_get_dirname(path, &length);

	if ( length < 1 || length >= MAX_PATH )
	{
		return false;
	}

	if ( path[length - 1] == '/' )
	{
		--length;
	}

	g_ExecutableDirectoryLength = length;
	memcpy(g_ExecutableDirectory, path, g_ExecutableDirectoryLength);
	g_ExecutableDirectory[g_ExecutableDirectoryLength + 1] = '\0';

	return true;
}

char* Platform_NativeAbsolutePathFromExecutableDirectory(Platform_Path relativePath)
{
	if ( !relativePath.path || !g_ExecutableDirectory[0] )
	{
		return NULL;
	}

	// We handle this manually:
	if ( relativePath.path[0] == '/' )
	{
		++relativePath.path;
	}

	size_t relLength = strlen(relativePath.path);
	size_t totalLength = g_ExecutableDirectoryLength + sizeof('/') + relLength + sizeof('\0');
	char* out = malloc(totalLength);

	memcpy(out, g_ExecutableDirectory, g_ExecutableDirectoryLength);
	out[g_ExecutableDirectoryLength] = '/';
	memcpy(out + g_ExecutableDirectoryLength + 1, relativePath.path, relLength);
	out[totalLength - 1] = '\0';

	return out;
}

struct Platform_DirectoryListing* Platform_GetDirectoryListing(Platform_Path path)
{
	char sPath[MAX_PATH];

	snprintf(sPath, sizeof(sPath), "%s\\*.*", path.path);
	sPath[MAX_PATH - 1] = '\0';

	WIN32_FIND_DATA fdFile;
	HANDLE hFind = FindFirstFile(sPath, &fdFile);

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

	listing->path = Platform_DuplicateString(path.path);

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

		listingEntry->name = Platform_DuplicateString(fdFile.cFileName);

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
	while ( FindNextFile(hFind, &fdFile) );

	FindClose(hFind);
	return listing;
}
