#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "RayGE/Platform.h"
#include "EngineSubsystems/FilesystemSubsystem.h"
#include "MemPool/MemPoolManager.h"
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

static FilesystemSubsystem_LongPath g_NativeRootDirectory;
static FilesystemSubsystem_Path g_BaseRelDirectory;

static void EnsureApplicationDirectory(bool forceRefresh)
{
	if ( g_NativeRootDirectory[0] && !forceRefresh )
	{
		return;
	}

	const char* appDir = GetApplicationDirectory();
	RAYGE_ENSURE(appDir && *appDir, "Could not retrieve application directory");

	size_t length = wzl_strcpy(g_NativeRootDirectory, sizeof(g_NativeRootDirectory), appDir);

	if ( !g_BaseRelDirectory[0] )
	{
		if ( g_NativeRootDirectory[length - 1] == PATH_SEP_CH )
		{
			// Make sure this is not present - we will manage it manually.
			g_NativeRootDirectory[length - 1] = '\0';
		}

		return;
	}

	if ( g_NativeRootDirectory[length - 1] != PATH_SEP_CH )
	{
		RAYGE_ENSURE(
			length < sizeof(g_NativeRootDirectory) - 1,
			"Not enough space to concatenate application directory with base path"
		);

		g_NativeRootDirectory[length - 1] = PATH_SEP_CH;
		++length;
	}

	RAYGE_ENSURE(
		length < sizeof(g_NativeRootDirectory) - 1,
		"Not enough space to concatenate application directory with base path"
	);

	length += wzl_strcpy(g_NativeRootDirectory + length, sizeof(g_NativeRootDirectory) - length, g_BaseRelDirectory);

	if ( g_NativeRootDirectory[length - 1] == PATH_SEP_CH )
	{
		// Make sure this is not present - we will manage it manually.
		g_NativeRootDirectory[length - 1] = '\0';
	}
}

// Caller takes ownership of the path.
static char* MakeAbsolutePathFromApplicationDirectory(const char* relNativePath)
{
	EnsureApplicationDirectory(false);

	if ( !relNativePath )
	{
		return wzl_strdup(g_NativeRootDirectory);
	}

	// If the path begins with a separator, treat it as being rooted at our current
	// root directory. This essentially means we need to just skip past any leading
	// separators and then compute the actual absolute path.
	while ( *relNativePath && *relNativePath == PATH_SEP_CH )
	{
		++relNativePath;
	}

	if ( !(*relNativePath) )
	{
		return wzl_strdup(g_NativeRootDirectory);
	}

	char dummy;
	size_t size = cwk_path_get_absolute(g_NativeRootDirectory, relNativePath, &dummy, sizeof(dummy)) + 1;

	RAYGE_ASSERT(size > 1, "Path concatenation produced empty path");

	char* buffer = MEMPOOL_MALLOC(MEMPOOL_FILESYSTEM, size);
	cwk_path_get_absolute(g_NativeRootDirectory, relNativePath, buffer, size);

	RAYGE_ASSERT(buffer[size - 1] == '\0', "Path length calculation was incorrect");

	return buffer;
}

// Caller takes ownership of the path
static char* MakeRelativePathFromApplicationDirectory(const char* absNativePath)
{
	EnsureApplicationDirectory(false);

	if ( !absNativePath || !(*absNativePath) )
	{
		return wzl_strdup("");
	}

	char dummy;
	size_t size = cwk_path_get_relative(g_NativeRootDirectory, absNativePath, &dummy, sizeof(dummy)) + 1;

	char* buffer = MEMPOOL_MALLOC(MEMPOOL_FILESYSTEM, size);
	cwk_path_get_relative(g_NativeRootDirectory, absNativePath, buffer, size);

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
	char* newPath = wzl_strdup(path ? path : "");

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
	char* newPath = wzl_strdup(path ? path : "");

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

static void FreeEntryContents(FilesystemSubsystem_PathEntry* entry)
{
	if ( entry && entry->path )
	{
		MEMPOOL_FREE(entry->path);
	}
}

void FilesystemSubsystem_Init(void)
{
	// Nothing yet.
	// Probably want to set up application directory here.
}

void FilesystemSubsystem_ShutDown(void)
{
	// Nothing yet.
}

FilesystemSubsystem_PathList* FilesystemSubsystem_ListDirectory(const char* path)
{
	char* nativeAbsPath = RelativePathToAbsoluteNativePath(path);
	FilePathList list = LoadDirectoryFiles(nativeAbsPath);
	MEMPOOL_FREE(nativeAbsPath);

	FilesystemSubsystem_PathList* outList = MEMPOOL_CALLOC_STRUCT(MEMPOOL_FILESYSTEM, FilesystemSubsystem_PathList);

	outList->count = (size_t)WZL_MAX(list.count, 0);

	if ( outList->count > 0 )
	{
		outList->entries = (FilesystemSubsystem_PathEntry*)
			MEMPOOL_CALLOC(MEMPOOL_FILESYSTEM, outList->count, sizeof(FilesystemSubsystem_PathEntry));

		for ( size_t index = 0; index < outList->count; ++index )
		{
			outList->entries[index].path = AbsoluteNativePathToRelativePath(list.paths[index]);
			outList->entries[index].isDirectory = !IsPathFile(outList->entries[index].path);
		}
	}

	return outList;
}

void FilesystemSubsystem_FreePathList(FilesystemSubsystem_PathList* list)
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

void FilesystemSubsystem_SetBaseRelPath(const char* path)
{
	if ( path && *path && PathBacktracksPastBaseDirectory(path) )
	{
		Logging_PrintLine(
			RAYGE_LOG_WARNING,
			"Cannot set filesystem base path to \"%s\" as this would leave engine filesystem subtree. Resetting to "
			"blank base path."
		);

		path = "";
	}

	if ( path )
	{
		wzl_strcpy(g_BaseRelDirectory, sizeof(g_BaseRelDirectory), path);
	}
	else
	{
		g_BaseRelDirectory[0] = '\0';
	}

	Logging_PrintLine(RAYGE_LOG_DEBUG, "Setting filesystem base path: %s", g_BaseRelDirectory);

	PathSeparatorsToNative(g_BaseRelDirectory);
	EnsureApplicationDirectory(true);

	RAYGE_ASSERT(DirectoryExists(g_NativeRootDirectory), "Specified root directory does not exist!");
}

bool FilesystemSubsystem_DirectoryExists(const char* path)
{
	char* nativePath = RelativePathToAbsoluteNativePath(path);
	const bool exists = DirectoryExists(nativePath);
	MEMPOOL_FREE(nativePath);

	return exists;
}

uint8_t* FilesystemSubsystem_LoadFileData(const char* path, size_t* size)
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

void FilesystemSubsystem_UnloadFileData(uint8_t* data)
{
	if ( data )
	{
		UnloadFileData(data);
	}
}

bool FilesystemSubsystem_MakeAbsolute(const char* relPath, char* outBuffer, size_t outBufferSize)
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

char* FilesystemSubsystem_MakeAbsoluteAlloc(const char* relPath)
{
	return relPath ? RelativePathToAbsoluteNativePath(relPath) : NULL;
}
