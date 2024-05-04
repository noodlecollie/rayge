#pragma once

#include <stddef.h>

typedef enum MemPool_Category
{
	MEMPOOL_UNCATEGORISED = 0,
	MEMPOOL_SCENE,
	MEMPOOL_ENTITY,

	MEMPOOL__COUNT
} MemPool_Category;

void MemPoolSubsystem_Init(void);
void MemPoolSubsystem_ShutDown(void);

// Reset once the subsystem is shut down.
// Defaults to true if the engine is built in debug mode,
// or false otherwise.
bool MemPoolSubsystem_DebuggingEnabled(void);
void MemPoolSubsystem_SetDebuggingEnabled(bool enabled);

void* MemPoolSubsystem_Malloc(const char* file, int line, MemPool_Category category, size_t size);
void* MemPoolSubsystem_Calloc(
	const char* file,
	int line,
	MemPool_Category category,
	size_t numElements,
	size_t elementSize
);
void* MemPoolSubsystem_Realloc(const char* file, int line, MemPool_Category category, void* memory, size_t newSize);
void MemPoolSubsystem_Free(const char* file, int line, void* memory);

// Prints information about the allocation to the logs.
// Debugging must be enabled (see MemPoolSubsystem_DebuggingEnabled()).
void MemPool_DumpAllocInfo(void* memory);

#define MEMPOOL_MALLOC(category, size) MemPoolSubsystem_Malloc(__FILE__, __LINE__, (category)(size))
#define MEMPOOL_CALLOC(category, numElements, elementSize) \
	MemPoolSubsystem_Calloc(__FILE__, __LINE__, (category), (numElements), (elementSize))
#define MEMPOOL_REALLOC(category, memory, newSize) \
	MemPoolSubsystem_Realloc(__FILE__, __LINE__, (category), (memory), (newSize))
#define MEMPOOL_FREE(memory) MemPoolSubsystem_Free(__FILE__, __LINE__, (memory))

#define MEMPOOL_MALLOC_STRUCT(category, type) ((type*)MEMPOOL_MALLOC(category, sizeof(type)))
#define MEMPOOL_CALLOC_STRUCT(category, type) ((type*)MEMPOOL_CALLOC(category, 1, sizeof(type)))
