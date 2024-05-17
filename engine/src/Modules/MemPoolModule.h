#pragma once

#include <stddef.h>
#include <stdbool.h>

// TODO: Move this to be its own thing, before modules are initialised,
// and set up wzl memory callbacks to use mempools.

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

	MEMPOOL__COUNT
} MemPool_Category;

void MemPoolModule_Init(void);
void MemPoolModule_ShutDown(void);

// Reset once the subsystem is shut down.
// Defaults to true if the engine is built in debug mode,
// or false otherwise.
bool MemPoolModule_DebuggingEnabled(void);
void MemPoolModule_SetDebuggingEnabled(bool enabled);

void* MemPoolModule_Malloc(const char* file, int line, MemPool_Category category, size_t size);
void* MemPoolModule_Calloc(
	const char* file,
	int line,
	MemPool_Category category,
	size_t numElements,
	size_t elementSize
);
void* MemPoolModule_Realloc(const char* file, int line, MemPool_Category category, void* memory, size_t newSize);
void MemPoolModule_Free(const char* file, int line, void* memory);

// Prints information about the allocation to the logs.
// Debugging must be enabled (see MemPoolModule_DebuggingEnabled()).
void MemPoolModule_DumpAllocInfo(void* memory);

#define MEMPOOL_MALLOC(category, size) MemPoolModule_Malloc(__FILE__, __LINE__, (category), (size))
#define MEMPOOL_CALLOC(category, numElements, elementSize) \
	MemPoolModule_Calloc(__FILE__, __LINE__, (category), (numElements), (elementSize))
#define MEMPOOL_REALLOC(category, memory, newSize) \
	MemPoolModule_Realloc(__FILE__, __LINE__, (category), (memory), (newSize))
#define MEMPOOL_FREE(memory) MemPoolModule_Free(__FILE__, __LINE__, (memory))

#define MEMPOOL_MALLOC_STRUCT(category, type) ((type*)MEMPOOL_MALLOC(category, sizeof(type)))
#define MEMPOOL_CALLOC_STRUCT(category, type) ((type*)MEMPOOL_CALLOC(category, 1, sizeof(type)))
