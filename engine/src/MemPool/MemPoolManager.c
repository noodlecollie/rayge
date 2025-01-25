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
#include "utlist.h"

// Disable some seemingly buggy GCC warnings for this file,
// since we will be doing some wacky things with pointers.
#ifndef _MSC_VER
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

#define HEAD_SENTINEL_VALUE 0xF9A23BAD
#define TAIL_SENTINEL_VALUE ~(HEAD_SENTINEL_VALUE)

#define ENSURE_INITIALISED() RAYGE_ENSURE(g_Initialised, "MemPool manager was not initialised")

static const char* const g_MemPoolNames[] = {
#define LIST_ITEM(enum, name) name,
	MEMPOOL_CATEGORY_LIST
#undef LIST_ITEM
};

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
	size_t totalClientMemory;  // Sizes of all allocations requested
	size_t totalMemory;  // Total memory used, including head and tail structs.
	size_t totalAllocations;
} MemPool;

typedef struct ManagerData
{
	MemPool pools[MEMPOOL__COUNT];
	bool debuggingEnabled;
} ManagerData;

static ManagerData g_Data;
static bool g_Initialised = false;

static const char* MemPoolName(MemPool_Category category)
{
	return (category >= MEMPOOL_UNCATEGORISED && category < MEMPOOL__COUNT) ? g_MemPoolNames[category] : "UNKNOWN";
}

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
		"Mem pool invocation from %s:%d: Head sentinel was trashed for 0x%p allocated from %s:%d. Expected 0x%08X, got "
		"0x%08X.",
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
		"Mem pool invocation from %s:%d: Tail sentinel was trashed for 0x%p allocated from %s:%d. Expected 0x%08X, got "
		"0x%08X.",
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

static void FreeChain(MemPoolItemHead** chain, const char* file, int line)
{
	MemPoolItemHead* item = NULL;
	MemPoolItemHead* temp1 = NULL;

	DL_FOREACH_SAFE(*chain, item, temp1)
	{
		VerifyIntegrity(item, file, line);
		free(item);
	}
}

static void CheckCountersForNewAllocation(MemPool* pool, size_t size, const char* file, int line)
{
	RAYGE_ENSURE(
		SIZE_MAX - pool->totalClientMemory >= size,
		"Mem pool invocation from %s:%d: Request to allocate %zu client bytes would overflow pool's client memory "
		"counter.",
		file,
		line,
		size
	);

	size_t totalSize = ItemAllocationSize(size);

	RAYGE_ENSURE(
		SIZE_MAX - pool->totalMemory >= totalSize,
		"Mem pool invocation from %s:%d: Request to allocate %zu client bytes would overflow pool's total memory "
		"counter.",
		file,
		line,
		size
	);

	RAYGE_ENSURE(
		pool->totalAllocations < SIZE_MAX,
		"Mem pool invocation from %s:%d: Request to allocate %zu client bytes would overflow pool's total allocations "
		"counter.",
		file,
		line,
		size
	);
}

static void CheckCountersForAllocationRemoval(MemPool* pool, size_t clientAllocSize, const char* file, int line)
{
	RAYGE_ENSURE(
		clientAllocSize <= pool->totalClientMemory,
		"Mem pool invocation from %s:%d: Freeing %zu bytes underflows pool's client memory counter.",
		file,
		line,
		clientAllocSize
	);

	const size_t totalBytesToFree = ItemAllocationSize(clientAllocSize);

	RAYGE_ENSURE(
		totalBytesToFree <= pool->totalMemory,
		"Mem pool invocation from %s:%d: Freeing %zu total bytes underflows pool's total memory counter.",
		file,
		line,
		totalBytesToFree
	);

	RAYGE_ENSURE(
		pool->totalAllocations > 0,
		"Mem pool invocation from %s:%d: Allocation to free but total allocations counter was zero.",
		file,
		line,
		totalBytesToFree
	);
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

	CheckCountersForNewAllocation(pool, size, file, line);

	const size_t totalSize = ItemAllocationSize(size);
	MemPoolItemHead* item = (MemPoolItemHead*)calloc(1, totalSize);

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

	DL_APPEND(pool->head, item);

	// Keep track of how much memory is being used in this pool.
	pool->totalClientMemory += item->allocSize;
	pool->totalMemory += totalSize;
	++pool->totalAllocations;

	return item;
}

static void DestroyItemInPool(MemPool* pool, MemPoolItemHead* item, const char* file, int line)
{
	CheckCountersForAllocationRemoval(pool, item->allocSize, file, line);

	const size_t totalBytesToFree = ItemAllocationSize(item->allocSize);

	pool->totalClientMemory -= item->allocSize;
	pool->totalMemory -= totalBytesToFree;
	--pool->totalAllocations;

	DL_DELETE(pool->head, item);

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
		FreeChain(&data->pools[index].head, __FILE__, __LINE__);
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
	CheckCountersForAllocationRemoval(item->pool, item->allocSize, file, line);

	DL_DELETE(item->pool->head, item);

	item->pool->totalClientMemory -= item->allocSize;
	item->pool->totalMemory -= ItemAllocationSize(item->allocSize);
	--item->pool->totalAllocations;

	CheckCountersForNewAllocation(item->pool, newSize, file, line);

	const size_t newPtrMemSize = ItemAllocationSize(newSize);
	void* newPtr = realloc(memory, newPtrMemSize);
	item = MemPtrToItem(newPtr);

	// Set up the item again. The head sentinel will still be intact,
	// but the tail will not.
	MemPoolItemTail* tail = ItemTail(item);
	tail->sentinel = TAIL_SENTINEL_VALUE;

	item->allocSize = newSize;
	item->allocFile = file;
	item->allocLine = line;

	DL_APPEND(item->pool->head, item);

	item->pool->totalClientMemory += item->allocSize;
	item->pool->totalMemory += newPtrMemSize;
	++item->pool->totalAllocations;

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
	LOG("  Head sentinel: 0x%08X", item->sentinel);
	LOG("  Tail sentinel: 0x%08X", tail->sentinel);
	LOG("  Pool: %s (%d)",
		item->pool ? MemPoolName(item->pool->category) : "UNKNOWN",
		item->pool ? (int)item->pool->category : -1);
	LOG("  Requested allocation size: %zu bytes", item->allocSize);
	LOG("  Allocated from: %s:%d", SafeFileNameString(item), SafeFileLineNumber(item));

#undef LOG
}

void MemPoolManager_DumpAllAllocInfo(void)
{
	ENSURE_INITIALISED();
	RAYGE_ASSERT(g_Data.debuggingEnabled, "Mem pool debugging must be enabled to use this function.");

	if ( !g_Data.debuggingEnabled )
	{
		return;
	}

	for ( size_t index = 0; index < MEMPOOL__COUNT; ++index )
	{
		MemPool* pool = &g_Data.pools[index];

		Logging_PrintLine(
			RAYGE_LOG_INFO,
			"Mempool %s (%zu) has %zu bytes allocated (%zu including overhead) across %zu allocations",
			MemPoolName((MemPool_Category)index),
			index,
			pool->totalClientMemory,
			pool->totalMemory,
			pool->totalAllocations
		);

		for ( MemPoolItemHead* item = pool->head; item; item = item->next )
		{
			MemPoolManager_DumpAllocInfo((uint8_t*)item + sizeof(*item));
		}

		Logging_PrintLine(RAYGE_LOG_INFO, "");
	}
}
