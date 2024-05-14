#include "Subsystems/CommandSubsystem.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Debugging.h"
#include "wzl_cutl/string.h"

#define UTHASH_POOLED_MEMPOOL MEMPOOL_COMMANDS
#include "UTUtils/UTHash_Pooled.h"

typedef struct CommandItem
{
	char* name;
	CommandSubsystem_Callback callback;
	void* userData;
	UT_hash_handle hh;
} CommandItem;

typedef struct CommandRegistry
{
	CommandItem* commandHash;
} CommandRegistry;

static CommandRegistry* g_Registry = NULL;

static void FreeCommandItem(CommandItem* item)
{
	if ( !item )
	{
		return;
	}

	if ( item->name )
	{
		MEMPOOL_FREE(item->name);
	}

	MEMPOOL_FREE(item);
}

static void FreeAllCommandItems(CommandRegistry* registry)
{
	CommandItem* item = NULL;
	CommandItem* tmp = NULL;

	HASH_ITER(hh, registry->commandHash, item, tmp)
	{
		HASH_DEL(registry->commandHash, item);
		MEMPOOL_FREE(item);
	}
}

static char* TrimCommandName(const char* name)
{
	const char* begin = NULL;
	const char* end = NULL;

	wzl_strtrimspace(name, &begin, &end);

	size_t nameLength = end - begin;

	if ( nameLength < 1 )
	{
		return NULL;
	}

	char* buffer = MEMPOOL_MALLOC(MEMPOOL_COMMANDS, nameLength + 1);
	memcpy(buffer, begin, nameLength);
	buffer[nameLength] = '\0';

	return buffer;
}

static CommandItem* FindCommandByName(CommandRegistry* registry, const char* name)
{
	CommandItem* item = NULL;
	HASH_FIND_STR(registry->commandHash, name, item);
	return item;
}

static void AddCommand(CommandRegistry* registry, char* name, CommandSubsystem_Callback callback, void* userData)
{
	CommandItem* item = MEMPOOL_CALLOC_STRUCT(MEMPOOL_COMMANDS, CommandItem);
	item->name = name;
	item->callback = callback;
	item->userData = userData;

	HASH_ADD_STR(registry->commandHash, name, item);
}

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

	FreeAllCommandItems(g_Registry);

	MEMPOOL_FREE(g_Registry);
	g_Registry = NULL;
}

bool CommandSubsystem_AddCommand(const char* name, CommandSubsystem_Callback callback, void* userData)
{
	RAYGE_ASSERT(g_Registry, "Command subsystem was not initialised");

	if ( !g_Registry )
	{
		return false;
	}

	if ( !name || !(*name) )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Failed to add command with invalid name");
		return false;
	}

	char* trimmedName = NULL;

	do
	{
		trimmedName = TrimCommandName(name);

		if ( FindCommandByName(g_Registry, name) )
		{
			LoggingSubsystem_PrintLine(
				RAYGE_LOG_ERROR,
				"Failed to add command \"%s\": a command with this name already exists.",
				trimmedName
			);

			break;
		}

		if ( !callback )
		{
			LoggingSubsystem_PrintLine(
				RAYGE_LOG_ERROR,
				"Failed to add command \"%s\": no callback was provided.",
				trimmedName
			);

			break;
		}

		AddCommand(g_Registry, trimmedName, callback, userData);
		return true;
	}
	while ( false );

	// Failure: clean up

	if ( trimmedName )
	{
		MEMPOOL_FREE(trimmedName);
	}

	return false;
}

bool CommandSubsystem_InvokeCommand(const char* commandName)
{
	RAYGE_ASSERT(g_Registry, "Command subsystem was not initialised");

	if ( !g_Registry )
	{
		return false;
	}

	if ( !commandName || !(*commandName) )
	{
		return false;
	}

	CommandItem* item = FindCommandByName(g_Registry, commandName);

	if ( !item || !item->callback )
	{
		return false;
	}

	item->callback(item->name, item->userData);
	return true;
}
