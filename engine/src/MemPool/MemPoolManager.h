#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "Testing/Testing.h"

#define MEMPOOL_CATEGORY_LIST \
	LIST_ITEM(MEMPOOL_UNCATEGORISED, "Uncategorised") \
	LIST_ITEM(MEMPOOL_RENDERER, "Renderer") \
	LIST_ITEM(MEMPOOL_SCENE, "Scene") \
	LIST_ITEM(MEMPOOL_ENTITY, "Entity") \
	LIST_ITEM(MEMPOOL_UI, "UI") \
	LIST_ITEM(MEMPOOL_INPUT, "Input") \
	LIST_ITEM(MEMPOOL_COMMANDS, "Commands") \
	LIST_ITEM(MEMPOOL_HOOKS, "Hooks") \
	LIST_ITEM(MEMPOOL_FILESYSTEM, "Filesystem") \
	LIST_ITEM(MEMPOOL_WZL_CUTL, "wzl_cutl") \
	LIST_ITEM(MEMPOOL_LOGGING, "Logging") \
	LIST_ITEM(MEMPOOL_RESOURCE_MANAGEMENT, "Resource Management") \
	LIST_ITEM(MEMPOOL_RAYLIB, "Raylib") \
	LIST_ITEM(MEMPOOL_TEST_MANAGER, "Test Manager") \
	LIST_ITEM(MEMPOOL__COUNT, "##COUNT##")

typedef enum MemPool_Category
{
#define LIST_ITEM(enum, name) enum,
	MEMPOOL_CATEGORY_LIST
#undef LIST_ITEM
} MemPool_Category;

void MemPoolManager_Init(void);
void MemPoolManager_ShutDown(void);

// Reset once the manager is shut down.
// Defaults to true if the engine is built in debug mode,
// or false otherwise.
bool MemPoolManager_DebuggingEnabled(void);
void MemPoolManager_SetDebuggingEnabled(bool enabled);

void* MemPoolManager_Malloc(const char* file, int line, MemPool_Category category, size_t size);
void* MemPoolManager_Calloc(
	const char* file,
	int line,
	MemPool_Category category,
	size_t numElements,
	size_t elementSize
);
void* MemPoolManager_Realloc(const char* file, int line, MemPool_Category category, void* memory, size_t newSize);
void MemPoolManager_Free(const char* file, int line, void* memory);

// Prints information about the allocation to the logs.
// Debugging must be enabled (see MemPoolManager_DebuggingEnabled()).
void MemPoolManager_DumpAllocInfo(void* memory);
void MemPoolManager_DumpAllAllocInfo(void);

#define MEMPOOL_MALLOC(category, size) MemPoolManager_Malloc(__FILE__, __LINE__, (category), (size))
#define MEMPOOL_CALLOC(category, numElements, elementSize) \
	MemPoolManager_Calloc(__FILE__, __LINE__, (category), (numElements), (elementSize))
#define MEMPOOL_REALLOC(category, memory, newSize) \
	MemPoolManager_Realloc(__FILE__, __LINE__, (category), (memory), (newSize))
#define MEMPOOL_FREE(memory) MemPoolManager_Free(__FILE__, __LINE__, (memory))

#define MEMPOOL_MALLOC_STRUCT(category, type) ((type*)MEMPOOL_MALLOC(category, sizeof(type)))
#define MEMPOOL_CALLOC_STRUCT(category, type) ((type*)MEMPOOL_CALLOC(category, 1, sizeof(type)))
