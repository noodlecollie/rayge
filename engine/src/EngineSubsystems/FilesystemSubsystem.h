#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "wzl_cutl/attributes.h"

#define FILESYSTEM_MAX_REL_PATH 512
#define FILESYSTEM_MAX_ABS_PATH 4096

// Convenience array for relative paths
typedef char FilesystemSubsystem_Path[FILESYSTEM_MAX_REL_PATH];

// Intended for full paths on the system
// (be careful of allocating this on the stack!)
typedef char FilesystemSubsystem_LongPath[FILESYSTEM_MAX_ABS_PATH];

typedef struct FilesystemSubsystem_PathEntry
{
	char* path;
	bool isDirectory;
} FilesystemSubsystem_PathEntry;

typedef struct FilesystemSubsystem_PathList
{
	size_t count;
	FilesystemSubsystem_PathEntry* entries;
} FilesystemSubsystem_PathList;

void FilesystemSubsystem_Init(void);
void FilesystemSubsystem_ShutDown(void);

FilesystemSubsystem_PathList* FilesystemSubsystem_ListDirectory(const char* path);
void FilesystemSubsystem_FreePathList(FilesystemSubsystem_PathList* list);

// Set the relative path from the application directory
// under which future calls will operate.
void FilesystemSubsystem_SetBaseRelPath(const char* path);

bool FilesystemSubsystem_DirectoryExists(const char* path);

WZL_ATTR_NODISCARD uint8_t* FilesystemSubsystem_LoadFileData(const char* path, size_t* size);
void FilesystemSubsystem_UnloadFileData(uint8_t* data);

bool FilesystemSubsystem_MakeAbsolute(const char* relPath, char* outBuffer, size_t outBufferSize);
WZL_ATTR_NODISCARD char* FilesystemSubsystem_MakeAbsoluteAlloc(const char* relPath);
