#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "Subsystems/MemPoolSubsystem.h"
#include "Debugging.h"

#define HEAD_SENTINEL_VALUE 0xF9A23BAD
#define TAIL_SENTINEL_VALUE ~(HEAD_SENTINEL_VALUE)

struct MemPool;

typedef struct MemPoolItemHead
{
	struct MemPool* pool;
	struct MemPoolItemHead* prev;
	struct MemPoolItemHead* next;

#if RAYGE_DEBUG()
	const char* allocFile;
	int allocLine;
#endif

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
} MemPool;

static MemPool g_Pools[MEMPOOL__COUNT];
static bool g_PoolsInitialised = false;

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
	const size_t poolMin = (size_t)g_Pools;
	const size_t poolMax = (size_t)(g_Pools + MEMPOOL__COUNT);

	return poolNum >= poolMin && poolNum < poolMax && poolNum % sizeof(void*) == 0;
}

static void VerifyIntegrity(MemPoolItemHead* item, const char* file, int line)
{
	RAYGE_ENSURE(
		item->sentinel != HEAD_SENTINEL_VALUE,
		"Mem pool invocation from %s:%d: Head sentinel was trashed for 0x%p",
		file,
		line,
		ItemToMemPtr(item)
	);

	RAYGE_ENSURE(
		VerifyPoolValid(item->pool),
		"Mem pool invocation from %s:%d: Mem pool pointer was trashed for 0x%p",
		file,
		line,
		ItemToMemPtr(item)
	);

	MemPoolItemTail* tail = ItemTail(item);

	RAYGE_ENSURE(
		tail->sentinel != TAIL_SENTINEL_VALUE,
		"Mem pool invocation from %s:%d: Tail sentinel was trashed for 0x%p",
		file,
		line,
		ItemToMemPtr(item)
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

static MemPoolItemHead* CreateItem(size_t size, const char* file, int line)
{
	MemPoolItemHead* head = (MemPoolItemHead*)malloc(ItemAllocationSize(size));
	MemPoolItemTail* tail = ItemTail(head);

	memset(head, 0, sizeof(*head));

	head->sentinel = HEAD_SENTINEL_VALUE;
	tail->sentinel = TAIL_SENTINEL_VALUE;

#if RAYGE_DEBUG()
	head->allocFile = file;
	head->allocLine = line;
#endif

	head->allocSize = size;

	return head;
}

static MemPoolItemHead* CreateItemInPool(MemPool* pool, size_t size, const char* file, int line)
{
	MemPoolItemHead* item = CreateItem(size, file, line);

	item->pool = pool;
	item->prev = pool->tail;

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

	return item;
}

void DestroyItemInPool(MemPool* pool, MemPoolItemHead* item)
{
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

void MemPoolSubsystem_Init()
{
	if ( g_PoolsInitialised )
	{
		return;
	}

	for ( size_t index = 0; index < MEMPOOL__COUNT; ++index )
	{
		memset(&g_Pools[index], 0, sizeof(MemPool));
		g_Pools[index].category = (MemPool_Category)index;
	}

	g_PoolsInitialised = true;
}

void MemPoolSubsystem_ShutDown()
{
	if ( !g_PoolsInitialised )
	{
		return;
	}

	for ( size_t index = 0; index < MEMPOOL__COUNT; ++index )
	{
		FreeChain(g_Pools[index].head, __FILE__, __LINE__);
		memset(&g_Pools[index], 0, sizeof(MemPool));
	}

	g_PoolsInitialised = false;
}

void* MemPoolSubsystem_Malloc(const char* file, int line, MemPool_Category category, size_t size)
{
	RAYGE_ENSURE(
		(size_t)category < MEMPOOL__COUNT,
		"%s:%d: Invalid category provided to MemPoolSubsystem_Malloc",
		file,
		line
	);

	MemPoolItemHead* head = CreateItemInPool(&g_Pools[(size_t)category], size, file, line);
	return ItemToMemPtr(head);
}

void* MemPoolSubsystem_Calloc(
	const char* file,
	int line,
	MemPool_Category category,
	size_t numElements,
	size_t elementSize
)
{
	RAYGE_ENSURE(
		(size_t)category < MEMPOOL__COUNT,
		"%s:%d: Invalid category provided to MemPoolSubsystem_Calloc",
		file,
		line
	);

	MemPoolItemHead* head = CreateItemInPool(&g_Pools[(size_t)category], numElements * elementSize, file, line);

	void* ptr = ItemToMemPtr(head);
	memset(ptr, 0, numElements * elementSize);

	return ptr;
}

void* MemPoolSubsystem_Realloc(const char* file, int line, MemPool_Category category, void* memory, size_t newSize)
{
	RAYGE_ENSURE(
		(size_t)category < MEMPOOL__COUNT,
		"%s:%d: Invalid category provided to MemPoolSubsystem_Realloc",
		file,
		line
	);

	if ( !memory )
	{
		return MemPoolSubsystem_Malloc(file, line, category, newSize);
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

void MemPoolSubsystem_Free(const char* file, int line, void* memory)
{
	RAYGE_ENSURE(memory, "%s:%d: Null pointer provided to MemPoolSubsystem_Free", file, line);

	MemPoolItemHead* item = MemPtrToItemChecked(memory, file, line);
	DestroyItemInPool(item->pool, item);
}