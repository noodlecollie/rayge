#include <stdbool.h>
#include <stddef.h>
#include "Hooks/HookManager.h"
#include "Hooks/MenuHooks.h"
#include "Utils.h"

typedef struct HookRegisterAndUnregister
{
	void (*Register)(void);
	void (*Unregister)(void);
} HookRegisterAndUnregister;

static const HookRegisterAndUnregister g_Hooks[] = {
	{MenuHooks_Register, NULL},
};

static bool g_Initialised = false;

void HookManager_RegisterAll(void)
{
	if ( g_Initialised )
	{
		return;
	}

	const HookRegisterAndUnregister* end = g_Hooks + RAYGE_ARRAY_SIZE(g_Hooks);

	for ( const HookRegisterAndUnregister* item = g_Hooks; item < end; ++item )
	{
		if ( item->Register )
		{
			item->Register();
		}
	}

	g_Initialised = true;
}

void HookManager_UnregisterAll(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	const HookRegisterAndUnregister* end = g_Hooks - 1;

	for ( const HookRegisterAndUnregister* item = g_Hooks + RAYGE_ARRAY_SIZE(g_Hooks) - 1; item > end; --item )
	{
		if ( item->Unregister )
		{
			item->Unregister();
		}
	}

	g_Initialised = false;
}
