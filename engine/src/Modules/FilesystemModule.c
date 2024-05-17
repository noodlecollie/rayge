#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "RayGE/Platform.h"
#include "Modules/FilesystemModule.h"
#include "Modules/MemPoolModule.h"
#include "Logging/Logging.h"
#include "Debugging.h"
#include "cwalk.h"
#include "wzl_cutl/string.h"
#include "wzl_cutl/math.h"

#if RAYGE_PLATFORM() == RAYGE_PLATFORM_WINDOWS
#define PATH_SEP_CH '\\'
#else
#define PATH_SEP_CH '/'
#endif

static FilesystemModule_LongPath g_NativeApplicationDirectory;

// TODO: Replace this with wzl_strdup once we've set up the allocators properly
static char* StrDup(const char* str)
{
	if ( !str )
	{
		return NULL;
	}

	const size_t size = strlen(str) + 1;
	char* out = MEMPOOL_MALLOC(MEMPOOL_FILESYSTEM, size);

	char* cursor = out;

	while ( *str )
	{
		*(cursor++) = *(str++);
	}

	*cursor = '\0';

	return out;
}

static void EnsureApplicationDirectory()
{
	if ( g_NativeApplicationDirectory[0] )
	{
		return;
	}

	const char* appDir = GetApplicationDirectory();
	RAYGE_ASSERT(appDir, "Could not retrieve application directory");

	if ( !appDir )
	{
		return;
	}

	wzl_strcpy(g_NativeApplicationDirectory, sizeof(g_NativeApplicationDirectory), appDir);
	RAYGE_ASSERT(g_NativeApplicationDirectory[0], "Received invalid application directory");

	if ( !g_NativeApplicationDirectory[0] )
	{
		return;
	}

	const size_t length = strlen(g_NativeApplicationDirectory);

	if ( g_NativeApplicationDirectory[length - 1] == PATH_SEP_CH )
	{
		// Make sure this is not present - we will manage it manually.
		g_NativeApplicationDirectory[length - 1] = '\0';
	}
}

// Caller takes ownership of the path.
static char* MakeAbsolutePathFromApplicationDirectory(const char* relNativePath)
{
	EnsureApplicationDirectory();

	if ( !relNativePath || !(*relNativePath) )
	{
		return StrDup(g_NativeApplicationDirectory);
	}

	char dummy;
	size_t size = cwk_path_get_absolute(g_NativeApplicationDirectory, relNativePath, &dummy, sizeof(dummy)) + 1;

	RAYGE_ASSERT(size > 1, "Path concatenation produced empty path");

	char* buffer = MEMPOOL_MALLOC(MEMPOOL_FILESYSTEM, size);
	cwk_path_get_absolute(g_NativeApplicationDirectory, relNativePath, buffer, size);

	RAYGE_ASSERT(buffer[size - 1] == '\0', "Path length calculation was incorrect");

	return buffer;
}

// Caller takes ownership of the path
static char* MakeRelativePathFromApplicationDirectory(const char* absNativePath)
{
	EnsureApplicationDirectory();

	if ( !absNativePath || !(*absNativePath) )
	{
		return StrDup("");
	}

	char dummy;
	size_t size = cwk_path_get_relative(g_NativeApplicationDirectory, absNativePath, &dummy, sizeof(dummy)) + 1;

	char* buffer = MEMPOOL_MALLOC(MEMPOOL_FILESYSTEM, size);
	cwk_path_get_relative(g_NativeApplicationDirectory, absNativePath, buffer, size);

	RAYGE_ASSERT(buffer[size - 1] == '\0', "Path length calculation was incorrect");

	return buffer;
}

static bool PathBacktracksPastBaseDirectory(const char* relNativePath)
{
	if ( !relNativePath )
	{
		return false;
	}

	size_t levelsDeep = 0;

	for ( const char *start = relNativePath, *end = strchr(start, PATH_SEP_CH); end;
		  start = end + 1, end = strchr(start, PATH_SEP_CH) )
	{
		if ( end == start )
		{
			// Found '/' immediately - no change.
			continue;
		}

		if ( end == start + 1 && *start == '.' )
		{
			// Current directory - no change.
			continue;
		}

		if ( end == start + 2 && start[0] == '.' && start[1] == '.' )
		{
			// This is an attempt to go back up one directory. Can we do so?
			if ( levelsDeep < 1 )
			{
				// Path goes past base directory.
				return true;
			}

			--levelsDeep;
			continue;
		}

		// Otherwise, the path segment represents changing to a deeper directory.
		++levelsDeep;
	}

	return false;
}

static void MakePathSafe(char* path)
{
	RAYGE_ASSERT(path, "Expected a valid path");

	if ( !path )
	{
		// Nothing we can do.
		return;
	}

	if ( path[0] == '/' || PathBacktracksPastBaseDirectory(path) )
	{
		path[0] = '\0';
	}
}

// Always returns a valid pointer.
static char* PathSeparatorsToNative(const char* path)
{
	char* newPath = StrDup(path ? path : "");

#if RAYGE_PLATFORM() == RAYGE_PLATFORM_WINDOWS
	for ( char* cursor = newPath; *cursor; ++cursor )
	{
		if ( *cursor == '/' )
		{
			*cursor = '\\';
		}
	}
#endif

	return newPath;
}

static char* PathSeparatorsFromNative(const char* path)
{
	char* newPath = StrDup(path ? path : "");

#if RAYGE_PLATFORM() == RAYGE_PLATFORM_WINDOWS
	for ( char* cursor = newPath; *cursor; ++cursor )
	{
		if ( *cursor == '\\' )
		{
			*cursor = '/';
		}
	}
#endif

	return newPath;
}

// TODO: Tidy up allocations here? This function is often used to
// compute a string, use it and then free the string later.
// It could probably be swapped out for a static buffer to reduce
// the number/frequency of dynamic allocations.
static char* RelativePathToAbsoluteNativePath(const char* relPath)
{
	char* nativeRelPath = PathSeparatorsToNative(relPath);
	MakePathSafe(nativeRelPath);

	char* nativeAbsPath = MakeAbsolutePathFromApplicationDirectory(nativeRelPath);

	MEMPOOL_FREE(nativeRelPath);
	return nativeAbsPath;
}

static char* AbsoluteNativePathToRelativePath(const char* absNativePath)
{
	char* relNativePath = MakeRelativePathFromApplicationDirectory(absNativePath);
	char* relPath = PathSeparatorsFromNative(relNativePath);

	MEMPOOL_FREE(relNativePath);
	return relPath;
}

static void FreeEntryContents(FilesystemModule_PathEntry* entry)
{
	if ( entry && entry->path )
	{
		MEMPOOL_FREE(entry->path);
	}
}

void FilesystemModule_Init(void)
{
	// Nothing yet.
	// Probably want to set up application directory here.
}

void FilesystemModule_ShutDown(void)
{
	// Nothing yet.
}

FilesystemModule_PathList* FilesystemModule_ListDirectory(const char* path)
{
	char* nativeAbsPath = RelativePathToAbsoluteNativePath(path);
	FilePathList list = LoadDirectoryFiles(nativeAbsPath);
	MEMPOOL_FREE(nativeAbsPath);

	FilesystemModule_PathList* outList = MEMPOOL_CALLOC_STRUCT(MEMPOOL_FILESYSTEM, FilesystemModule_PathList);

	outList->count = (size_t)WZL_MAX(list.count, 0);

	if ( outList->count > 0 )
	{
		outList->entries = (FilesystemModule_PathEntry*)
			MEMPOOL_CALLOC(MEMPOOL_FILESYSTEM, outList->count, sizeof(FilesystemModule_PathEntry));

		for ( size_t index = 0; index < outList->count; ++index )
		{
			outList->entries[index].path = AbsoluteNativePathToRelativePath(list.paths[index]);
			outList->entries[index].isDirectory = !IsPathFile(outList->entries[index].path);
		}
	}

	return outList;
}

void FilesystemModule_FreePathList(FilesystemModule_PathList* list)
{
	if ( !list )
	{
		return;
	}

	for ( size_t index = 0; index < list->count; ++index )
	{
		FreeEntryContents(&list->entries[index]);
	}

	MEMPOOL_FREE(list->entries);
	MEMPOOL_FREE(list);
}

bool FilesystemModule_DirectoryExists(const char* path)
{
	char* nativePath = RelativePathToAbsoluteNativePath(path);
	const bool exists = DirectoryExists(nativePath);
	MEMPOOL_FREE(nativePath);

	return exists;
}

uint8_t* FilesystemModule_LoadFileData(const char* path, size_t* size)
{
	char* nativePath = RelativePathToAbsoluteNativePath(path);

	int dataSize = 0;
	uint8_t* data = LoadFileData(nativePath, &dataSize);

	MEMPOOL_FREE(nativePath);

	if ( size )
	{
		*size = (size_t)WZL_MAX(dataSize, 0);
	}

	return data;
}

void FilesystemModule_UnloadFileData(uint8_t* data)
{
	if ( data )
	{
		UnloadFileData(data);
	}
}

bool FilesystemModule_MakeAbsolute(const char* relPath, char* outBuffer, size_t outBufferSize)
{
	if ( !outBuffer || outBufferSize < 1 )
	{
		return false;
	}

	outBuffer[0] = '\0';

	if ( !relPath || !(*relPath) )
	{
		return false;
	}

	char* nativePath = RelativePathToAbsoluteNativePath(relPath);
	wzl_strcpy(outBuffer, outBufferSize, nativePath);
	MEMPOOL_FREE(nativePath);

	return true;
}
