#include <stdbool.h>
#include <stddef.h>
#include "Hooks/HookManager.h"
#include "Utils/Utils.h"
#include "Headless.h"

#if !RAYGE_HEADLESS()
#include "Non-Headless/Hooks/MenuHooks.h"
#endif

typedef struct HookRegisterAndUnregister
{
	void (*Register)(void);
	void (*Unregister)(void);
} HookRegisterAndUnregister;

// TODO: Remove once we have something non-headless to put in the hooks array
static void DummyInit(void)
{
}

static void DummyShutDown(void)
{
}

static const HookRegisterAndUnregister g_Hooks[] = {
#if !RAYGE_HEADLESS()
	{MenuHooks_Register, MenuHooks_Unregister},
#endif
	{DummyInit, DummyShutDown},
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
