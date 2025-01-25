// Declared in the RaylibAllocators.h precompiled header file

#include <stddef.h>
#include "MemPool/MemPoolManager.h"

void* RaylibCustomMalloc(size_t size, const char* file, int line)
{
	return MemPoolManager_Malloc(file, line, MEMPOOL_RAYLIB, size);
}

void* RaylibCustomCalloc(size_t numItems, size_t itemSize, const char* file, int line)
{
	return MemPoolManager_Calloc(file, line, MEMPOOL_RAYLIB, numItems, itemSize);
}

void* RaylibCustomRealloc(void* mem, size_t newSize, const char* file, int line)
{
	return MemPoolManager_Realloc(file, line, MEMPOOL_RAYLIB, mem, newSize);
}

void RaylibCustomFree(void* mem, const char* file, int line)
{
	// Sometimes STB sends a null pointer in through here, so check first
	if ( mem )
	{
		MemPoolManager_Free(file, line, mem);
	}
}
