#pragma once

typedef enum Platform_FileNodeType
{
	PLATFORM_NODE_UNKNOWN = 0,
	PLATFORM_NODE_FILE = 0,
	PLATFORM_NODE_DIRECTORY
} Platform_FileNodeType;

struct Platform_DirectoryListing;
struct Platform_DirectoryEntry;

struct Platform_DirectoryListing* Platform_GetDirectoryListing(const char* directory);
void Platform_FreeDirectoryListing(struct Platform_DirectoryListing* listing);

const char* Platform_DirectoryListing_GetDirectoryPath(struct Platform_DirectoryListing* listing);
struct Platform_DirectoryEntry* Platform_DirectoryListing_GetFirstEntry(struct Platform_DirectoryListing* listing);
struct Platform_DirectoryEntry* Platform_DirectoryListing_GetPrevEntry(struct Platform_DirectoryEntry* entry);
struct Platform_DirectoryEntry* Platform_DirectoryListing_GetNextEntry(struct Platform_DirectoryEntry* entry);
Platform_FileNodeType Platform_DirectoryListing_GetNodeType(struct Platform_DirectoryEntry* entry);
const char* Platform_DirectoryListing_GetNodeName(struct Platform_DirectoryEntry* entry);
