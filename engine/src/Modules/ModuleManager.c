#include <stdbool.h>
#include "Modules/ModuleManager.h"
#include "Modules/FilesystemModule.h"
#include "Modules/RendererModule.h"
#include "Modules/InputModule.h"
#include "Modules/UIModule.h"
#include "Modules/CommandModule.h"
#include "Modules/InputHookModule.h"
#include "Modules/SceneModule.h"
#include "Utils.h"

typedef struct ModuleInitAndShutdown
{
	void (*Init)(void);
	void (*ShutDown)(void);
} ModuleInitAndShutdown;

static const ModuleInitAndShutdown g_Modules[] = {
	{FilesystemModule_Init, FilesystemModule_ShutDown},
	{RendererModule_Init, RendererModule_ShutDown},
	{SceneModule_Init, SceneModule_ShutDown},
	{InputModule_Init, InputModule_ShutDown},
	{InputHookModule_Init, InputHookModule_ShutDown},
	{UIModule_Init, UIModule_ShutDown},
	{CommandModule_Init, CommandModule_ShutDown},
};

static bool g_Initialised = false;

void ModuleManager_InitAll(void)
{
	if ( g_Initialised )
	{
		return;
	}

	const ModuleInitAndShutdown* end = g_Modules + RAYGE_ARRAY_SIZE(g_Modules);

	for ( const ModuleInitAndShutdown* subsystem = g_Modules; subsystem < end; ++subsystem )
	{
		if ( subsystem->Init )
		{
			subsystem->Init();
		}
	}

	g_Initialised = true;
}

void ModuleManager_ShutDownAll(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	const ModuleInitAndShutdown* end = g_Modules - 1;

	for ( const ModuleInitAndShutdown* subsystem = g_Modules + RAYGE_ARRAY_SIZE(g_Modules) - 1; subsystem > end;
		  --subsystem )
	{
		if ( subsystem->ShutDown )
		{
			subsystem->ShutDown();
		}
	}

	g_Initialised = false;
}
