#include "Subsystems/CommandSubsystem.h"
#include "Subsystems/MemPoolSubsystem.h"

#define UTHASH_POOLED_MEMPOOL MEMPOOL_COMMANDS
#include "UTUtils/UTHash_Pooled.h"

typedef struct CommandItem
{
	UT_hash_handle hh;
} CommandItem;

typedef struct CommandRegistry
{
	CommandItem* commandHash;
} CommandRegistry;

static CommandRegistry* g_Registry = NULL;

void CommandSubsystem_Init(void)
{
	if ( g_Registry )
	{
		return;
	}

	g_Registry = MEMPOOL_CALLOC_STRUCT(MEMPOOL_COMMANDS, CommandRegistry);
}

void CommandSubsystem_ShutDown(void)
{
	if ( !g_Registry )
	{
		return;
	}

	MEMPOOL_FREE(g_Registry);
	g_Registry = NULL;
}
