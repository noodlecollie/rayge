#include <sys/types.h>
#include <dirent.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Platform/FileSystem.h"
#include "Platform/String.h"
#include "Common/FileSystemCommon.h"
#include "cwalk.h"

static char g_ExecutableDirectory[PATH_MAX];
static size_t g_ExecutableDirectoryLength = 0;

static DIR* OpenDir(Platform_Path path)
{
	char* nativeDirectory = Platform_NativeAbsolutePathFromExecutableDirectory(path);

	if ( !nativeDirectory )
	{
		return NULL;
	}

	DIR* dirPtr = opendir(nativeDirectory);
	free(nativeDirectory);
	return dirPtr;
}

bool Platform_SetExecutableFromArgV0(const char* path)
{
	if ( !path )
	{
		return false;
	}

	char temp[PATH_MAX];

	if ( cwk_path_is_relative(path) )
	{
		char cwd[PATH_MAX];

		if ( !getcwd(cwd, sizeof(cwd)) )
		{
			return false;
		}

		cwk_path_get_absolute(cwd, path, temp, sizeof(temp));
		path = temp;
	}

	size_t length = 0;
	cwk_path_get_dirname(path, &length);

	if ( length < 1 || length >= PATH_MAX )
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

	listing->path = Platform_DuplicateString(path.path);

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
