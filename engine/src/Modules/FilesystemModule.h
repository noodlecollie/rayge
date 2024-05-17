#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define FILESYSTEM_MAX_REL_PATH 512
#define FILESYSTEM_MAX_ABS_PATH 4096

// Convenience array for relative paths
typedef char FilesystemModule_Path[FILESYSTEM_MAX_REL_PATH];

// Intended for full paths on the system
// (be careful of allocating this on the stack!)
typedef char FilesystemModule_LongPath[FILESYSTEM_MAX_ABS_PATH];

typedef struct FilesystemModule_PathEntry
{
	char* path;
	bool isDirectory;
} FilesystemModule_PathEntry;

typedef struct FilesystemModule_PathList
{
	size_t count;
	FilesystemModule_PathEntry* entries;
} FilesystemModule_PathList;

void FilesystemModule_Init(void);
void FilesystemModule_ShutDown(void);

FilesystemModule_PathList* FilesystemModule_ListDirectory(const char* path);
void FilesystemModule_FreePathList(FilesystemModule_PathList* list);

// TODO: Function to set base path

bool FilesystemModule_DirectoryExists(const char* path);

uint8_t* FilesystemModule_LoadFileData(const char* path, size_t* size);
void FilesystemModule_UnloadFileData(uint8_t* data);

bool FilesystemModule_MakeAbsolute(const char* relPath, char* outBuffer, size_t outBufferSize);
