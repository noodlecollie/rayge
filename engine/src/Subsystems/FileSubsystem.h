#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Convenience array for relative paths
typedef char FileSubsystem_Path[512];

// Intended for full paths on the system
// (be careful of allocating this on the stack!)
typedef char FileSubsystem_LongPath[4096];

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
