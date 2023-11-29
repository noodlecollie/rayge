#pragma once

#include "Platform/FileSystem.h"

typedef struct Platform_DirectoryEntry
{
	struct Platform_DirectoryEntry* prev;
	struct Platform_DirectoryEntry* next;
	Platform_FileNodeType type;
	char* name;
} Platform_DirectoryEntry;

typedef struct Platform_DirectoryListing
{
	Platform_DirectoryEntry* entries;
	char* path;
} Platform_DirectoryListing;

void FileSystem_LinuxToWindowsPathSeparators(char* path);
