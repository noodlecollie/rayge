#include "Non-Headless/Hooks/MenuHooks.h"
#include "Non-Headless/EngineSubsystems/UISubsystem.h"
#include "Logging/Logging.h"
#include "EngineSubsystems/CommandSubsystem.h"
#include "MemPool/MemPoolManager.h"
#include "EngineSubsystems/InputHookSubsystem.h"
#include "Input/KeyboardModifiers.h"
#include "Non-Headless/UI/SceneDebugUI.h"
#include "Non-Headless/UI/ImGuiDemo.h"
#include "Non-Headless/UI/DeveloperConsole.h"
#include "Non-Headless/UI/ResourceViewer.h"
#include "Debugging.h"
#include "wzl_cutl/string.h"
#include "utlist.h"
#include "raylib.h"

typedef struct StateItem
{
	struct StateItem* next;
	const CommandSubsystem_CommandHandle* showCmd;
	const CommandSubsystem_CommandHandle* hideCmd;
	const CommandSubsystem_CommandHandle* toggleCmd;
	const RayGE_UIMenu* menu;
} StateItem;

static StateItem* g_State = NULL;
static bool g_Registered = false;

static void ShutDownAllMenus(void)
{
	StateItem* item = NULL;

	LL_FOREACH(g_State, item)
	{
		if ( item->menu->ShutDown )
		{
			item->menu->ShutDown(item->menu->userData);
		}
	}
}

static void DeleteAllStateItems(void)
{
	StateItem* item = NULL;
	StateItem* temp = NULL;

	LL_FOREACH_SAFE(g_State, item, temp)
	{
		LL_DELETE(g_State, item);
		MEMPOOL_FREE(item);
	}
}

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
		UISubsystem_ShowMenu(menu);
	}
	else if ( commandName[0] == '-' )
	{
		Logging_PrintLine(RAYGE_LOG_DEBUG, "Hide menu command: %s", commandName);
		UISubsystem_HideMenu(menu);
	}
	else
	{
		Logging_PrintLine(RAYGE_LOG_DEBUG, "Toggle menu command: %s", commandName);

		if ( UISubsystem_IsMenuActive(menu) )
		{
			UISubsystem_HideMenu(menu);
		}
		else
		{
			UISubsystem_ShowMenu(menu);
		}
	}
}

static void HandleHook(RayGE_InputSource source, int id, const RayGE_InputBuffer* inputBuffer, void* userData)
{
	(void)inputBuffer;

	const StateItem* state = (const StateItem*)userData;

	if ( state->toggleCmd )
	{
		Logging_PrintLine(RAYGE_LOG_TRACE, "Toggling menu for source %d key %d", source, id);
		CommandSubsystem_InvokeCommand(state->toggleCmd);
	}
}

static void RegisterMenuCommands(StateItem* state, const char* name, const RayGE_UIMenu* menu)
{
	char fullName[32];

	wzl_sprintf(fullName, sizeof(fullName), "+%s", name);
	const CommandSubsystem_CommandHandle* showCmd = CommandSubsystem_AddCommand(fullName, HandleCommand, (void*)menu);

	if ( state )
	{
		state->showCmd = showCmd;
	}

	wzl_sprintf(fullName, sizeof(fullName), "-%s", name);
	const CommandSubsystem_CommandHandle* hideCmd = CommandSubsystem_AddCommand(fullName, HandleCommand, (void*)menu);

	if ( state )
	{
		state->hideCmd = hideCmd;
	}

	const CommandSubsystem_CommandHandle* toggleCmd = CommandSubsystem_AddCommand(name, HandleCommand, (void*)menu);

	if ( state )
	{
		state->toggleCmd = toggleCmd;
	}
}

static void RegisterMenu(int key, unsigned int modifierFlags, const char* name, const RayGE_UIMenu* menu)
{
	RAYGE_ENSURE(name && menu, "Expected a valid name and menu");

	if ( key != KEY_NULL )
	{
		Logging_PrintLine(
			RAYGE_LOG_TRACE,
			"Registering menu hook for %s on key %d (modifier condition: 0x%08x)",
			name,
			key,
			modifierFlags
		);
	}
	else
	{
		Logging_PrintLine(RAYGE_LOG_TRACE, "Registering menu %s", name);
	}

	StateItem* state = MEMPOOL_CALLOC_STRUCT(MEMPOOL_HOOKS, StateItem);
	LL_PREPEND(g_State, state);

	state->menu = menu;

	// Register the relevant commands for this menu (show, hide, toggle).
	// These can be invoked manually from the console if desired.
	RegisterMenuCommands(state, name, menu);

	if ( key != KEY_NULL )
	{
		const RayGE_InputHook hook = {
			.triggerFlags = INPUT_TRIGGER_ACTIVE | INPUT_TRIGGER_OVERRIDE_UI_FOCUS,
			.callback = HandleHook,
			.userData = state
		};

		// Register the input hook for the menu. This will invoke the relevant
		// command when the key is pressed.
		InputHookSubsystem_AddHook(INPUT_SOURCE_KEYBOARD, key, modifierFlags, hook);
	}

	if ( state->menu->Init )
	{
		state->menu->Init(state->menu->userData);
	}
}

static void RegisterMenuCommandOnly(const char* name, const RayGE_UIMenu* menu)
{
	RegisterMenu(KEY_NULL, KEYMOD_NONE, name, menu);
}

static void RegisterMenus(void)
{
	RegisterMenu(KEY_GRAVE, KEYMOD_REQUIRE_NONE, "Engine.Menu.DeveloperConsole", &Menu_DeveloperConsole);
	RegisterMenu(KEY_GRAVE, KEYMOD_CTRL, "Engine.Menu.Debug", &Menu_SceneDebugUI);
	RegisterMenu(KEY_GRAVE, KEYMOD_CTRL | KEYMOD_ALT, "Engine.Menu.ImGuiDemo", &Menu_ImGuiDemo);

	RegisterMenuCommandOnly("Engine.Menu.ResourceViewer", &Menu_ResourceViewer);
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

	ShutDownAllMenus();
	DeleteAllStateItems();

	// Just in case:
	g_State = NULL;

	g_Registered = false;
}
