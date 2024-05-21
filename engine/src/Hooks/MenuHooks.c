#include "Hooks/MenuHooks.h"
#include "Modules/UIModule.h"
#include "Logging/Logging.h"
#include "Modules/CommandModule.h"
#include "MemPool/MemPoolManager.h"
#include "Modules/InputHookModule.h"
#include "Input/KeyboardModifiers.h"
#include "UI/SceneDebugUI.h"
#include "UI/TestUI.h"
#include "Debugging.h"
#include "wzl_cutl/string.h"
#include "utlist.h"

typedef struct StateItem
{
	struct StateItem* next;
	const CommandModule_CommandHandle* showCmd;
	const CommandModule_CommandHandle* hideCmd;
	const RayGE_UIMenu* menu;
} StateItem;

static StateItem* g_State = NULL;
static bool g_Registered = false;

static void HandleCommand(const char* commandName, void* userData)
{
	RAYGE_ENSURE(commandName, "Expected valid command name");

	if ( !userData )
	{
		Logging_PrintLine(RAYGE_LOG_WARNING, "MenuHooks: Expected a valid menu for command hook!");
		return;
	}

	const RayGE_UIMenu* menu = (const RayGE_UIMenu*)userData;

	if ( commandName[0] == '+' )
	{
		Logging_PrintLine(RAYGE_LOG_DEBUG, "Show menu command: %s", commandName);
		UIModule_SetCurrentMenu(menu);
	}
	else if ( commandName[0] == '-' )
	{
		Logging_PrintLine(RAYGE_LOG_DEBUG, "Hide menu command: %s", commandName);
		UIModule_ClearCurrentMenu();
	}
	else
	{
		Logging_PrintLine(RAYGE_LOG_WARNING, "MenuHooks: Unsupported command name syntax \"%s\"", commandName);
	}
}

static void HandleHook(RayGE_InputSource source, int id, const RayGE_InputBuffer* inputBuffer, void* userData)
{
	(void)inputBuffer;

	const StateItem* state = (const StateItem*)userData;

	if ( state->menu && UIModule_GetCurrentMenu() != state->menu )
	{
		Logging_PrintLine(RAYGE_LOG_TRACE, "Triggering show menu for source %d key %d", source, id);
		CommandModule_InvokeCommand(state->showCmd);
	}
	else
	{
		Logging_PrintLine(RAYGE_LOG_TRACE, "Triggering hide menu for source %d key %d", source, id);
		CommandModule_InvokeCommand(state->hideCmd);
	}
}

static void RegisterMenuCommands(StateItem* state, const char* name, const RayGE_UIMenu* menu)
{
	char fullName[32];

	wzl_sprintf(fullName, sizeof(fullName), "+%s", name);
	const CommandModule_CommandHandle * showCmd = CommandModule_AddCommand(fullName, HandleCommand, (void*)menu);

	if ( state )
	{
		state->showCmd = showCmd;
	}

	wzl_sprintf(fullName, sizeof(fullName), "-%s", name);
	const CommandModule_CommandHandle * hideCmd = CommandModule_AddCommand(fullName, HandleCommand, (void*)menu);

	if ( state )
	{
		state->hideCmd = hideCmd;
	}
}

static void RegisterMenuWithModifiers(int key, unsigned int modifierFlags, const char* name, const RayGE_UIMenu* menu)
{
	RAYGE_ENSURE(name && menu, "Expected a valid name and menu");

	Logging_PrintLine(
		RAYGE_LOG_TRACE,
		"Registering menu hook for %s on key %d (modifier condition: 0x%08x)",
		name,
		key,
		modifierFlags
	);

	StateItem* state = MEMPOOL_CALLOC_STRUCT(MEMPOOL_HOOKS, StateItem);
	LL_PREPEND(g_State, state);

	state->menu = menu;
	RegisterMenuCommands(state, name, menu);

	const RayGE_InputHook hook = {
		.triggerFlags = INPUT_TRIGGER_ACTIVE | INPUT_TRIGGER_OVERRIDE_UI_FOCUS,
		.callback = HandleHook,
		.userData = state
	};

	InputHookModule_AddHook(INPUT_SOURCE_KEYBOARD, key, modifierFlags, hook);
}

// static void RegisterMenu(int key, const char* name, const RayGE_UIMenu* menu)
// {
// 	RegisterMenuWithModifiers(key, KEYMOD_NONE, name, menu);
// }

static void RegisterMenus(void)
{
	RegisterMenuWithModifiers(KEY_GRAVE, KEYMOD_CTRL, "menu_debug", &Menu_SceneDebugUI);

	// Menus which are not bound to specific keys, but can be shown by executing commands manually:
	RegisterMenuCommands(NULL, "menu_testui", &Menu_TestUI);
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
