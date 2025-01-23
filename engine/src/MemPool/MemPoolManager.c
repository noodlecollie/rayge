#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include "MemPool/MemPoolManager.h"
#include "Launcher/LaunchParams.h"
#include "Logging/Logging.h"
#include "Debugging.h"
#include "Utils/Utils.h"

// Disable some seemingly buggy GCC warnings for this file,
// since we will be doing some wacky things with pointers.
#ifndef _MSC_VER
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

#define HEAD_SENTINEL_VALUE 0xF9A23BAD
#define TAIL_SENTINEL_VALUE ~(HEAD_SENTINEL_VALUE)

#define ENSURE_INITIALISED() RAYGE_ENSURE(g_Initialised, "MemPool manager was not initialised")

struct MemPool;

typedef struct MemPoolItemHead
{
	struct MemPool* pool;
	struct MemPoolItemHead* prev;
	struct MemPoolItemHead* next;

	const char* allocFile;
	int allocLine;
	size_t allocSize;
	uint32_t sentinel;
} MemPoolItemHead;

typedef struct MemPoolItemTail
{
	uint32_t sentinel;
} MemPoolItemTail;

typedef struct MemPool
{
	MemPool_Category category;
	MemPoolItemHead* head;
	MemPoolItemHead* tail;
	size_t totalClientMemory;  // Sizes of all allocations requested
	size_t totalMemory;  // Total memory used, including head and tail structs.
} MemPool;

typedef struct ManagerData
{
	MemPool pools[MEMPOOL__COUNT];
	bool debuggingEnabled;
} ManagerData;

static ManagerData g_Data;
static bool g_Initialised = false;

static const char* SafeFileNameString(MemPoolItemHead* item)
{
	static const char* const CORRUPTED = "<corrupted>";
	static const size_t MAX_LENGTH = 128;

	if ( !item || !item->allocFile )
	{
		return "<null>";
	}

	for ( size_t index = 0; index < MAX_LENGTH; ++index )
	{
		if ( !item->allocFile[index] )
		{
			// We found a null, so everything was OK.
			return item->allocFile;
		}

		if ( !isprint(item->allocFile[index]) )
		{
			return CORRUPTED;
		}
	}

	return CORRUPTED;
}

static int SafeFileLineNumber(MemPoolItemHead* item)
{
	return item ? item->allocLine : 0;
}

static void* ItemToMemPtr(MemPoolItemHead* item)
{
	return (void*)((uint8_t*)item + sizeof(MemPoolItemHead));
}

static MemPoolItemTail* ItemTail(MemPoolItemHead* item)
{
	return (MemPoolItemTail*)((uint8_t*)item + sizeof(MemPoolItemHead) + item->allocSize);
}

static bool VerifyPoolValid(MemPool* pool)
{
	const size_t poolNum = (size_t)pool;
	const size_t poolMin = (size_t)g_Data.pools;
	const size_t poolMax = (size_t)(g_Data.pools + MEMPOOL__COUNT);

	return poolNum >= poolMin && poolNum < poolMax && poolNum % sizeof(void*) == 0;
}

static void VerifyIntegrity(MemPoolItemHead* item, const char* file, int line)
{
	if ( !g_Data.debuggingEnabled )
	{
		return;
	}

	RAYGE_ENSURE(
		item->sentinel == HEAD_SENTINEL_VALUE,
		"Mem pool invocation from %s:%d: Head sentinel was trashed for 0x%p allocated from %s:%d. Expected 0x%08x, got "
		"0x%08x.",
		file,
		line,
		ItemToMemPtr(item),
		SafeFileNameString(item),
		SafeFileLineNumber(item),
		HEAD_SENTINEL_VALUE,
		item->sentinel
	);

	RAYGE_ENSURE(
		VerifyPoolValid(item->pool),
		"Mem pool invocation from %s:%d: Mem pool pointer was trashed for 0x%p allocated from %s:%d.",
		file,
		line,
		ItemToMemPtr(item),
		SafeFileNameString(item),
		SafeFileLineNumber(item)
	);

	MemPoolItemTail* tail = ItemTail(item);

	RAYGE_ENSURE(
		tail->sentinel == TAIL_SENTINEL_VALUE,
		"Mem pool invocation from %s:%d: Tail sentinel was trashed for 0x%p allocated from %s:%d. Expected 0x%08x, got "
		"0x%08x.",
		file,
		line,
		ItemToMemPtr(item),
		SafeFileNameString(item),
		SafeFileLineNumber(item),
		TAIL_SENTINEL_VALUE,
		tail->sentinel
	);
}

static MemPoolItemHead* MemPtrToItem(void* ptr)
{
	return (MemPoolItemHead*)((uint8_t*)ptr - sizeof(MemPoolItemHead));
}

static MemPoolItemHead* MemPtrToItemChecked(void* ptr, const char* file, int line)
{
	MemPoolItemHead* item = MemPtrToItem(ptr);
	VerifyIntegrity(item, file, line);
	return item;
}

static size_t ItemAllocationSize(size_t coreSize)
{
	return sizeof(MemPoolItemHead) + coreSize + sizeof(MemPoolItemTail);
}

static void FreeChain(MemPoolItemHead* chain, const char* file, int line)
{
	while ( chain )
	{
		VerifyIntegrity(chain, file, line);

		MemPoolItemHead* next = chain->next;
		free(chain);
		chain = next;
	}
}

static MemPoolItemHead* CreateItemInPool(MemPool* pool, size_t size, const char* file, int line)
{
	RAYGE_ENSURE(size > 0, "Mem pool invocation from %s:%d: Invalid request to allocate zero bytes", file, line);

	RAYGE_ENSURE(
		SIZE_MAX - pool->totalClientMemory >= size,
		"Mem pool invocation from %s:%d: Request to allocate %zu bytes would overflow pool's client memory counter.",
		file,
		line,
		size
	);

	size_t totalSize = ItemAllocationSize(size);

	RAYGE_ENSURE(
		SIZE_MAX - pool->totalMemory >= totalSize,
		"Mem pool invocation from %s:%d: Request to allocate %zu total bytes would overflow pool's total memory "
		"counter.",
		file,
		line,
		size
	);

	MemPoolItemHead* item = (MemPoolItemHead*)malloc(totalSize);

	RAYGE_ENSURE(
		item,
		"Mem pool invocation from %s:%d: Could not allocate %zu total bytes (%zu requested as payload)",
		file,
		line,
		totalSize,
		size
	);

	item->sentinel = HEAD_SENTINEL_VALUE;
	item->allocSize = size;
	item->allocFile = file;
	item->allocLine = line;

	ItemTail(item)->sentinel = TAIL_SENTINEL_VALUE;

	item->pool = pool;
	item->prev = pool->tail;
	item->next = NULL;

	if ( pool->tail )
	{
		pool->tail->next = item;
	}
	else
	{
		pool->tail = item;
	}

	if ( !pool->head )
	{
		// Pool was empty, so this is the first item.
		pool->head = item;
	}

	// Keep track of how much memory is being used in this pool.
	pool->totalClientMemory += item->allocSize;
	pool->totalMemory += totalSize;

	return item;
}

static void DestroyItemInPool(MemPool* pool, MemPoolItemHead* item, const char* file, int line)
{
	// If this happens, something's gone seriously wrong:
	RAYGE_ENSURE(
		item->allocSize <= pool->totalClientMemory,
		"Mem pool invocation from %s:%d: Freeing %zu bytes underflows pool's client memory counter.",
		file,
		line,
		item->allocSize
	);

	size_t totalBytesToFree = ItemAllocationSize(item->allocSize);

	// If this happens, something's gone seriously wrong:
	RAYGE_ENSURE(
		totalBytesToFree <= pool->totalMemory,
		"Mem pool invocation from %s:%d: Freeing %zu total bytes underflows pool's total memory counter.",
		file,
		line,
		totalBytesToFree
	);

	pool->totalClientMemory -= item->allocSize;
	pool->totalMemory -= ItemAllocationSize(item->allocSize);

	if ( item->prev )
	{
		item->prev->next = item->next;
	}

	if ( item->next )
	{
		item->next->prev = item->prev;
	}

	if ( pool->head == item )
	{
		pool->head = item->next;
	}

	if ( pool->tail == item )
	{
		pool->tail = item->prev;
	}

	free(item);
}

static void InitData(ManagerData* data)
{
	for ( size_t index = 0; index < RAYGE_ARRAY_SIZE(data->pools); ++index )
	{
		memset(&data->pools[index], 0, sizeof(MemPool));
		data->pools[index].category = (MemPool_Category)index;
	}

	data->debuggingEnabled = LaunchParams_GetLaunchState()->enableMemPoolDebugging;

	if ( data->debuggingEnabled )
	{
		Logging_PrintLine(RAYGE_LOG_WARNING, "Mempool debugging is enabled. This may affect performance.");
	}
}

static void FreeData(ManagerData* data)
{
	for ( size_t index = 0; index < RAYGE_ARRAY_SIZE(data->pools); ++index )
	{
		FreeChain(data->pools[index].head, __FILE__, __LINE__);
	}

	memset(data, 0, sizeof(*data));
}

bool MemPoolManager_DebuggingEnabled(void)
{
	return g_Data.debuggingEnabled;
}

void MemPoolManager_SetDebuggingEnabled(bool enabled)
{
	g_Data.debuggingEnabled = g_Initialised && enabled;
}

void MemPoolManager_Init(void)
{
	if ( g_Initialised )
	{
		return;
	}

	InitData(&g_Data);
	g_Initialised = true;
}

void MemPoolManager_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	FreeData(&g_Data);
	g_Initialised = false;
}

void* MemPoolManager_Malloc(const char* file, int line, MemPool_Category category, size_t size)
{
	ENSURE_INITIALISED();

	RAYGE_ENSURE(
		(size_t)category < MEMPOOL__COUNT,
		"Mem pool invocation from %s:%d: Invalid category provided to MemPoolManager_Malloc",
		file,
		line
	);

	MemPoolItemHead* head = CreateItemInPool(&g_Data.pools[(size_t)category], size, file, line);
	return ItemToMemPtr(head);
}

void* MemPoolManager_Calloc(
	const char* file,
	int line,
	MemPool_Category category,
	size_t numElements,
	size_t elementSize
)
{
	ENSURE_INITIALISED();

	RAYGE_ENSURE(
		(size_t)category < MEMPOOL__COUNT,
		"Mem pool invocation from %s:%d: Invalid category provided to MemPoolManager_Calloc",
		file,
		line
	);

	MemPoolItemHead* head = CreateItemInPool(&g_Data.pools[(size_t)category], numElements * elementSize, file, line);

	void* ptr = ItemToMemPtr(head);
	memset(ptr, 0, numElements * elementSize);

	return ptr;
}

void* MemPoolManager_Realloc(const char* file, int line, MemPool_Category category, void* memory, size_t newSize)
{
	ENSURE_INITIALISED();

	RAYGE_ENSURE(
		(size_t)category < MEMPOOL__COUNT,
		"Mem pool invocation from %s:%d: Invalid category provided to MemPoolManager_Realloc",
		file,
		line
	);

	if ( !memory )
	{
		return MemPoolManager_Malloc(file, line, category, newSize);
	}

	MemPoolItemHead* item = MemPtrToItemChecked(memory, file, line);
	MemPoolItemHead* prev = item->prev;
	MemPoolItemHead* next = item->next;

	void* newPtr = realloc(memory, ItemAllocationSize(newSize));

	// Fix the final sentinel which will likely have been corrupted.
	// The first one should be OK since that memory will have migrated.
	item = MemPtrToItem(newPtr);
	MemPoolItemTail* tail = ItemTail(item);
	tail->sentinel = TAIL_SENTINEL_VALUE;

	if ( prev )
	{
		prev->next = item;
	}
	else
	{
		item->pool->head = item;
	}

	if ( next )
	{
		next->prev = item;
	}
	else
	{
		item->pool->tail = item;
	}

	return newPtr;
}

void MemPoolManager_Free(const char* file, int line, void* memory)
{
	ENSURE_INITIALISED();
	RAYGE_ENSURE(memory, "Mem pool invocation from %s:%d: Null pointer provided to MemPoolManager_Free", file, line);

	MemPoolItemHead* item = MemPtrToItemChecked(memory, file, line);
	DestroyItemInPool(item->pool, item, file, line);
}

void MemPoolManager_DumpAllocInfo(void* memory)
{
	ENSURE_INITIALISED();
	RAYGE_ASSERT(g_Data.debuggingEnabled, "Mem pool debugging must be enabled to use this function.");

	if ( !g_Data.debuggingEnabled )
	{
		return;
	}

	RAYGE_ENSURE(memory, "Cannot dump allocation info for null pointer.");

	MemPoolItemHead* item = MemPtrToItem(memory);
	MemPoolItemTail* tail = ItemTail(item);

#define LOG(...) Logging_PrintLine(RAYGE_LOG_INFO, __VA_ARGS__)

	LOG("==== Allocation info for 0x%p ====", memory);
	LOG("  Head sentinel: 0x%08x", item->sentinel);
	LOG("  Tail sentinel: 0x%08x", tail->sentinel);
	LOG("  Pool: %d", item->pool ? (int)item->pool->category : -1);
	LOG("  Requested allocation size: %zu bytes", item->allocSize);
	LOG("  Allocated from: %s:%d", SafeFileNameString(item), SafeFileLineNumber(item));

#undef LOG
}
