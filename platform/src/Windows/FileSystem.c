#include <stdlib.h>
#include <stdio.h>
#include "Platform/FileSystem.h"
#include "Common/FileSystemCommon.h"
#include "cwalk.h"
#include "wzl_cutl/string.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <direct.h>

static Platform_Path g_ExecutableDirectory;
static size_t g_ExecutableDirectoryLength = 0;

bool Platform_SetExecutableFromArgV0(const char* nativePath)
{
	if ( !nativePath )
	{
		return false;
	}

	Platform_Path temp;

	if ( cwk_path_is_relative(nativePath) )
	{
		Platform_Path cwd;

		if ( !_getcwd(cwd, sizeof(cwd)) )
		{
			return false;
		}

		cwk_path_get_absolute(cwd, nativePath, temp, sizeof(temp));
		nativePath = temp;
	}

	size_t length = 0;
	cwk_path_get_dirname(nativePath, &length);

	if ( length < 1 || length >= sizeof(Platform_Path) )
	{
		return false;
	}

	if ( nativePath[length - 1] == '/' )
	{
		--length;
	}

	g_ExecutableDirectoryLength = length;
	memcpy(g_ExecutableDirectory, nativePath, g_ExecutableDirectoryLength);
	g_ExecutableDirectory[g_ExecutableDirectoryLength + 1] = '\0';

	return true;
}

bool Platform_DirectoryExists(const char* path)
{
	if ( !path || !(*path) )
	{
		return false;
	}

	char* nativePath = Platform_NativeAbsolutePathFromExecutableDirectory(path);
	const DWORD attributes = GetFileAttributesA(nativePath);
	free(nativePath);

	return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool Platform_FileExists(const char* path)
{
	if ( !path || !(*path) )
	{
		return false;
	}

	char* nativePath = Platform_NativeAbsolutePathFromExecutableDirectory(path);
	const DWORD attributes = GetFileAttributesA(nativePath);
	free(nativePath);

	return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

char* Platform_NativeAbsolutePathFromExecutableDirectory(const char* relativePath)
{
	if ( !relativePath || !g_ExecutableDirectory[0] )
	{
		return NULL;
	}

	// We handle this manually:
	if ( relativePath[0] == '/' )
	{
		++relativePath;
	}

	size_t relLength = strlen(relativePath);
	size_t totalLength = g_ExecutableDirectoryLength + sizeof('/') + relLength + sizeof('\0');
	char* out = malloc(totalLength);

	memcpy(out, g_ExecutableDirectory, g_ExecutableDirectoryLength);
	out[g_ExecutableDirectoryLength] = '/';
	memcpy(out + g_ExecutableDirectoryLength + 1, relativePath, relLength);
	out[totalLength - 1] = '\0';

	return out;
}

struct Platform_DirectoryListing* Platform_GetDirectoryListing(const char* path)
{
	Platform_Path sPath;

	snprintf(sPath, sizeof(sPath), "%s\\*.*", path);
	sPath[sizeof(Platform_Path) - 1] = '\0';

	WIN32_FIND_DATA fdFile;
	HANDLE hFind = FindFirstFileA(sPath, &fdFile);

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
