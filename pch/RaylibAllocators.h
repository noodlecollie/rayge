#pragma once

#include <stddef.h>

// These functions are assumed to be implemented in the engine.
extern void* RaylibCustomMalloc(size_t size, const char* file, int line);
extern void* RaylibCustomCalloc(size_t numItems, size_t itemSize, const char* file, int line);
extern void* RaylibCustomRealloc(void* mem, size_t newSize, const char* file, int line);
extern void RaylibCustomFree(void* mem, const char* file, int line);

#define RL_MALLOC(sz) RaylibCustomMalloc((sz), __FILE__, __LINE__)
#define RL_CALLOC(n, sz) RaylibCustomCalloc((n), (sz), __FILE__, __LINE__)
#define RL_REALLOC(n, sz) RaylibCustomRealloc((n), (sz), __FILE__, __LINE__)
#define RL_FREE(p) RaylibCustomFree((p), __FILE__, __LINE__)

#define STBTT_malloc(sz, u) RL_MALLOC(sz)
#define STBTT_free(p, u) RL_FREE(p)
