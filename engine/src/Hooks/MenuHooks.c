#include "Hooks/MenuHooks.h"
#include "Subsystems/UISubsystem.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Subsystems/CommandSubsystem.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Subsystems/InputHookSubsystem.h"
#include "Input/KeyboardModifiers.h"
#include "UI/TestUI.h"
#include "Debugging.h"
#include "wzl_cutl/string.h"
#include "utlist.h"

typedef struct StateItem
{
	struct StateItem* next;
	const CommandSubsystem_CommandHandle* showCmd;
	const CommandSubsystem_CommandHandle* hideCmd;
	const RayGE_UIMenu* menu;
} StateItem;

static StateItem* g_State = NULL;
static bool g_Registered = false;

static void HandleCommand(const char* commandName, void* userData)
{
	RAYGE_ENSURE(commandName, "Expected valid command name");

	if ( !userData )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_WARNING, "MenuHooks: Expected a valid menu for command hook!");
		return;
	}

	const RayGE_UIMenu* menu = (const RayGE_UIMenu*)userData;

	if ( commandName[0] == '+' )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_DEBUG, "Showing menu: %s", commandName + 1);
		UISubsystem_SetCurrentMenu(menu);
	}
	else if ( commandName[0] == '-' )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_DEBUG, "Hiding menu: %s", commandName + 1);
		UISubsystem_ClearCurrentMenu();
	}
	else
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_WARNING, "MenuHooks: Unsupported command name syntax \"%s\"", commandName);
	}
}

static void HandleHook(RayGE_InputSource source, int id, const RayGE_InputBuffer* inputBuffer, void* userData)
{
	(void)inputBuffer;

	const StateItem* state = (const StateItem*)userData;

	if ( state->menu && UISubsystem_GetCurrentMenu() != state->menu )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_TRACE, "Triggering show menu for source %d key %d", source, id);
		CommandSubsystem_InvokeCommand(state->showCmd);
	}
	else
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_TRACE, "Triggering hide menu for source %d key %d", source, id);
		CommandSubsystem_InvokeCommand(state->hideCmd);
	}
}

static void RegisterMenuWithModifiers(int key, unsigned int modifierFlags, const char* name, const RayGE_UIMenu* menu)
{
	RAYGE_ENSURE(name && menu, "Expected a valid name and menu");

	StateItem* state = MEMPOOL_CALLOC_STRUCT(MEMPOOL_HOOKS, StateItem);
	LL_PREPEND(g_State, state);

	state->menu = menu;

	char fullName[32];

	wzl_sprintf(fullName, sizeof(fullName), "+%s", name);
	state->showCmd = CommandSubsystem_AddCommand(fullName, HandleCommand, (void*)menu);

	wzl_sprintf(fullName, sizeof(fullName), "-%s", name);
	state->hideCmd = CommandSubsystem_AddCommand(fullName, HandleCommand, (void*)menu);

	const RayGE_InputHook hook = {
		.triggerFlags = INPUT_TRIGGER_ACTIVE | INPUT_TRIGGER_OVERRIDE_UI_FOCUS,
		.callback = HandleHook,
		.userData = state
	};

	InputHookSubsystem_AddHook(INPUT_SOURCE_KEYBOARD, key, modifierFlags, hook);
}

static void RegisterMenu(int key, const char* name, const RayGE_UIMenu* menu)
{
	RegisterMenuWithModifiers(key, KEYMOD_NONE, name, menu);
}

static void RegisterMenus(void)
{
	RegisterMenuWithModifiers(KEY_GRAVE, KEYMOD_CTRL, "menu_testui", &Menu_TestUI);
}

void MenuHooks_Register(void)
{
	if ( g_Registered )
	{
		return;
	}

	RegisterMenus();
	g_Registered = true;
}

void MenuHooks_Unregister(void)
{
	if ( !g_Registered )
	{
		return;
	}

	StateItem* item = NULL;
	StateItem* temp = NULL;

	LL_FOREACH_SAFE(g_State, item, temp)
	{
		LL_DELETE(g_State, item);
		MEMPOOL_FREE(item);
	}

	// Just in case:
	g_State = NULL;

	g_Registered = false;
}
