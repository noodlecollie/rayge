#pragma once

#include <stddef.h>

typedef enum MemPool_Category
{
	MEMPOOL_GENERAL = 0,

	MEMPOOL__COUNT
} MemPool_Category;

void MemPoolSubsystem_Init(void);
void MemPoolSubsystem_ShutDown(void);

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

#define MEMPOOL_MALLOC(size) MemPoolSubsystem_Malloc(__FILE__, __LINE__, (size))
#define MEMPOOL_CALLOC(numElements, elementSize) \
	MemPoolSubsystem_Calloc(__FILE__, __LINE__, (numElements), (elementSize))
#define MEMPOOL_REALLOC(memory, newSize) MemPoolSubsystem_Realloc(__FILE__, __LINE__, (memory), (newSize))
#define MEMPOOL_FREE(memory) MemPoolSubsystem_Free(__FILE__, __LINE__, (memory))
