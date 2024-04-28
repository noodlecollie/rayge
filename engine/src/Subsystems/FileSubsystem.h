#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define FILESYSTEM_MAX_REL_PATH 512
#define FILESYSTEM_MAX_ABS_PATH 4096

// Convenience array for relative paths
typedef char FileSubsystem_Path[FILESYSTEM_MAX_REL_PATH];

// Intended for full paths on the system
// (be careful of allocating this on the stack!)
typedef char FileSubsystem_LongPath[FILESYSTEM_MAX_ABS_PATH];

typedef struct FileSubsystem_PathEntry
{
	char* path;
	bool isDirectory;
} FileSubsystem_PathEntry;

typedef struct FileSubsystem_PathList
{
	size_t count;
	FileSubsystem_PathEntry* entries;
} FileSubsystem_PathList;

FileSubsystem_PathList* FileSubsystem_ListDirectory(const char* path);
void FileSubsystem_FreePathList(FileSubsystem_PathList* list);

// TODO: Function to set base path

bool FileSubsystem_DirectoryExists(const char* path);

uint8_t* FileSubsystem_LoadFileData(const char* path, size_t* size);
void FileSubsystem_UnloadFileData(uint8_t* data);

bool FileSubsystem_MakeAbsolute(const char* relPath, char* outBuffer, size_t outBufferSize);
