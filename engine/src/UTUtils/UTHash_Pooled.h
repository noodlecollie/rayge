#ifdef UTHASH_POOLED_INCLUDED
#error UTHash_Pooled.h has already been included elsewhere!
#endif

#ifdef UTHASH_H
#error uthash.h has already been included elsewhere!
#endif

#define UTHASH_POOLED_INCLUDED

#ifndef UTHASH_POOLED_MEMPOOL
#error UTHASH_POOLED_MEMPOOL was not defined! Define this to point to a specific MemPool_Category constant.
#endif

#include "MemPool/MemPoolManager.h"
#include "Debugging.h"

#define uthash_malloc(sz) MEMPOOL_MALLOC(UTHASH_POOLED_MEMPOOL, (sz))
#define uthash_free(ptr, sz) MEMPOOL_FREE((ptr))
#define uthash_fatal(msg) RAYGE_FATAL(msg)

#include "uthash.h"
