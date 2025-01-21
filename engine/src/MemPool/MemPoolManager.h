#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "Testing/Testing.h"

typedef enum MemPool_Category
{
	MEMPOOL_UNCATEGORISED = 0,
	MEMPOOL_RENDERER,
	MEMPOOL_SCENE,
	MEMPOOL_ENTITY,
	MEMPOOL_UI,
	MEMPOOL_INPUT,
	MEMPOOL_COMMANDS,
	MEMPOOL_HOOKS,
	MEMPOOL_FILESYSTEM,
	MEMPOOL_WZL_CUTL,
	MEMPOOL_LOGGING,
	MEMPOOL_RESOURCE_MANAGEMENT,

#if RAYGE_BUILD_TESTING()
	// Special pool for test manager usage
	MEMPOOL_TESTMANAGER,
#endif

	MEMPOOL__COUNT
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

#define MEMPOOL_MALLOC(category, size) MemPoolManager_Malloc(__FILE__, __LINE__, (category), (size))
#define MEMPOOL_CALLOC(category, numElements, elementSize) \
	MemPoolManager_Calloc(__FILE__, __LINE__, (category), (numElements), (elementSize))
#define MEMPOOL_REALLOC(category, memory, newSize) \
	MemPoolManager_Realloc(__FILE__, __LINE__, (category), (memory), (newSize))
#define MEMPOOL_FREE(memory) MemPoolManager_Free(__FILE__, __LINE__, (memory))

#define MEMPOOL_MALLOC_STRUCT(category, type) ((type*)MEMPOOL_MALLOC(category, sizeof(type)))
#define MEMPOOL_CALLOC_STRUCT(category, type) ((type*)MEMPOOL_CALLOC(category, 1, sizeof(type)))
