#include "Hooks/MenuHooks.h"
#include "Subsystems/UISubsystem.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Subsystems/CommandSubsystem.h"
#include "UI/TestUI.h"
#include "Debugging.h"
#include "wzl_cutl/string.h"

static void HandleHook(const char* commandName, void* userData)
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

static void RegisterMenu(const char* name, const RayGE_UIMenu* menu)
{
	RAYGE_ENSURE(name && menu, "Expected a valid name and menu");

	char fullName[32];

	wzl_sprintf(fullName, sizeof(fullName), "+%s", name);
	CommandSubsystem_AddCommand(fullName, HandleHook, (void*)menu);

	wzl_sprintf(fullName, sizeof(fullName), "-%s", name);
	CommandSubsystem_AddCommand(fullName, HandleHook, (void*)menu);
}

void MenuHooks_Register(void)
{
	RegisterMenu("menu_testui", &Menu_TestUI);
}
