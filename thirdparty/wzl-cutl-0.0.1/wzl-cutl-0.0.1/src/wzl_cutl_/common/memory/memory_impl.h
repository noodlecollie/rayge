#ifndef WZL_CUTL_COMMON_MEMORY_MEMORY_IMPL_H
#define WZL_CUTL_COMMON_MEMORY_MEMORY_IMPL_H

#include <stdlib.h>
#include "wzl_cutl/memory.h"

extern wzl_memory_delegates g_memory_delegates;

static inline void* wzl_malloc_(size_t size)
{
	return g_memory_delegates.malloc_func ? g_memory_delegates.malloc_func(size) : malloc(size);
}

static inline void wzl_free_(void* ptr)
{
	if ( g_memory_delegates.free_func )
	{
		g_memory_delegates.free_func(ptr);
	}
	else
	{
		free(ptr);
	}
}

static inline void* wzl_calloc_(size_t num, size_t size)
{
	return g_memory_delegates.calloc_func ? g_memory_delegates.calloc_func(num, size) : calloc(num, size);
}

static inline void* wzl_realloc_(void* ptr, size_t size)
{
	return g_memory_delegates.realloc_func ? g_memory_delegates.realloc_func(ptr, size) : realloc(ptr, size);
}

#define WZL_MALLOC_STRUCT(typeName) ((typeName)wzl_malloc_(sizeof(typeName)))
#define WZL_CALLOC_STRUCT(typeName) ((typeName)wzl_calloc_(1, sizeof(typeName)))

#endif  // WZL_CUTL_COMMON_MEMORY_MEMORY_IMPL_H
