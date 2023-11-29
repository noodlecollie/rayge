#pragma once

#include <stdbool.h>

typedef enum Platform_FileNodeType
{
	PLATFORM_NODE_UNKNOWN = 0,
	PLATFORM_NODE_FILE = 0,
	PLATFORM_NODE_DIRECTORY
} Platform_FileNodeType;

struct Platform_DirectoryListing;
struct Platform_DirectoryEntry;

// Always expected to be relative, and to use forward slash as a separator.
typedef struct Platform_Path
{
	char* path;
} Platform_Path;

Platform_Path Platform_AllocatePath(const char* path);
void Platform_FreePath(Platform_Path path);
bool Platform_PathIsAbsolute(Platform_Path path);

bool Platform_SetExecutableFromArgV0(const char* path);

// Assumes Platform_SetExecutableFromArgV0() has been called successfully.
// Caller takes ownership of the native path. It must be freed later.
char* Platform_NativeAbsolutePathFromExecutableDirectory(Platform_Path relativePath);

struct Platform_DirectoryListing* Platform_GetDirectoryListing(Platform_Path path);
void Platform_FreeDirectoryListing(struct Platform_DirectoryListing* listing);

const char* Platform_DirectoryListing_GetDirectoryPath(struct Platform_DirectoryListing* listing);
struct Platform_DirectoryEntry* Platform_DirectoryListing_GetFirstEntry(struct Platform_DirectoryListing* listing);
struct Platform_DirectoryEntry* Platform_DirectoryListing_GetPrevEntry(struct Platform_DirectoryEntry* entry);
struct Platform_DirectoryEntry* Platform_DirectoryListing_GetNextEntry(struct Platform_DirectoryEntry* entry);
Platform_FileNodeType Platform_DirectoryListing_GetNodeType(struct Platform_DirectoryEntry* entry);
const char* Platform_DirectoryListing_GetNodeName(struct Platform_DirectoryEntry* entry);
