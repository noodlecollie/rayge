#include "Modules/CommandModule.h"
#include "Modules/MemPoolModule.h"
#include "Logging/Logging.h"
#include "Debugging.h"
#include "wzl_cutl/string.h"

#define UTHASH_POOLED_MEMPOOL MEMPOOL_COMMANDS
#include "UTUtils/UTHash_Pooled.h"

struct CommandModule_CommandHandle
{
	char* name;
	CommandModule_Callback callback;
	void* userData;
	UT_hash_handle hh;
};

typedef struct CommandRegistry
{
	CommandModule_CommandHandle* commandHash;
} CommandRegistry;

static CommandRegistry* g_Registry = NULL;

static void FreeCommandItem(CommandModule_CommandHandle* item)
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
	CommandModule_CommandHandle* item = NULL;
	CommandModule_CommandHandle* tmp = NULL;

	HASH_ITER(hh, registry->commandHash, item, tmp)
	{
		HASH_DEL(registry->commandHash, item);
		FreeCommandItem(item);
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

static CommandModule_CommandHandle* FindCommandByName(CommandRegistry* registry, const char* name)
{
	CommandModule_CommandHandle* item = NULL;
	HASH_FIND_STR(registry->commandHash, name, item);
	return item;
}

static CommandModule_CommandHandle*
AddCommand(CommandRegistry* registry, char* name, CommandModule_Callback callback, void* userData)
{
	Logging_PrintLine(RAYGE_LOG_TRACE, "Adding command: %s", name);

	CommandModule_CommandHandle* item = MEMPOOL_CALLOC_STRUCT(MEMPOOL_COMMANDS, CommandModule_CommandHandle);
	item->name = name;
	item->callback = callback;
	item->userData = userData;

	HASH_ADD_STR(registry->commandHash, name, item);

	return item;
}

void CommandModule_Init(void)
{
	if ( g_Registry )
	{
		return;
	}

	g_Registry = MEMPOOL_CALLOC_STRUCT(MEMPOOL_COMMANDS, CommandRegistry);
}

void CommandModule_ShutDown(void)
{
	if ( !g_Registry )
	{
		return;
	}

	FreeAllCommandItems(g_Registry);

	MEMPOOL_FREE(g_Registry);
	g_Registry = NULL;
}

const CommandModule_CommandHandle*
CommandModule_AddCommand(const char* name, CommandModule_Callback callback, void* userData)
{
	RAYGE_ASSERT_VALID(g_Registry);

	if ( !g_Registry )
	{
		return NULL;
	}

	if ( !name || !(*name) )
	{
		Logging_PrintLine(RAYGE_LOG_ERROR, "Failed to add command with invalid name");
		return NULL;
	}

	char* trimmedName = NULL;

	do
	{
		trimmedName = TrimCommandName(name);

		if ( FindCommandByName(g_Registry, name) )
		{
			Logging_PrintLine(
				RAYGE_LOG_ERROR,
				"Failed to add command \"%s\": a command with this name already exists.",
				trimmedName
			);

			break;
		}

		if ( !callback )
		{
			Logging_PrintLine(
				RAYGE_LOG_ERROR,
				"Failed to add command \"%s\": no callback was provided.",
				trimmedName
			);

			break;
		}

		return AddCommand(g_Registry, trimmedName, callback, userData);
	}
	while ( false );

	// Failure: clean up

	if ( trimmedName )
	{
		MEMPOOL_FREE(trimmedName);
	}

	return NULL;
}

const CommandModule_CommandHandle* CommandModule_FindCommand(const char* commandName)
{
	RAYGE_ASSERT_VALID(g_Registry);

	if ( !g_Registry )
	{
		return NULL;
	}

	if ( !commandName || !(*commandName) )
	{
		return NULL;
	}

	return FindCommandByName(g_Registry, commandName);
}

void CommandModule_InvokeCommand(const CommandModule_CommandHandle* command)
{
	RAYGE_ASSERT_VALID(g_Registry);

	if ( !g_Registry || !command )
	{
		return;
	}

	if ( command->callback )
	{
		command->callback(command->name, command->userData);
	}
}
