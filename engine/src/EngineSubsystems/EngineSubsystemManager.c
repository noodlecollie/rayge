#include <stdbool.h>
#include "EngineSubsystems/EngineSubsystemManager.h"
#include "EngineSubsystems/FilesystemSubsystem.h"
#include "EngineSubsystems/RendererSubsystem.h"
#include "EngineSubsystems/InputSubsystem.h"
#include "EngineSubsystems/UISubsystem.h"
#include "EngineSubsystems/CommandSubsystem.h"
#include "EngineSubsystems/InputHookSubsystem.h"
#include "EngineSubsystems/SceneSubsystem.h"
#include "Utils.h"

typedef struct SubsystemInitAndShutdown
{
	void (*Init)(void);
	void (*ShutDown)(void);
} SubsystemInitAndShutdown;

static const SubsystemInitAndShutdown g_Subsystems[] = {
	{FilesystemSubsystem_Init, FilesystemSubsystem_ShutDown},
	{RendererSubsystem_Init, RendererSubsystem_ShutDown},
	{SceneSubsystem_Init, SceneSubsystem_ShutDown},
	{InputSubsystem_Init, InputSubsystem_ShutDown},
	{InputHookSubsystem_Init, InputHookSubsystem_ShutDown},
	{UISubsystem_Init, UISubsystem_ShutDown},
	{CommandSubsystem_Init, CommandSubsystem_ShutDown},
};

static bool g_Initialised = false;

void EngineSubsystemManager_InitAll(void)
{
	if ( g_Initialised )
	{
		return;
	}

	const SubsystemInitAndShutdown* end = g_Subsystems + RAYGE_ARRAY_SIZE(g_Subsystems);

	for ( const SubsystemInitAndShutdown* subsystem = g_Subsystems; subsystem < end; ++subsystem )
	{
		if ( subsystem->Init )
		{
			subsystem->Init();
		}
	}

	g_Initialised = true;
}

void EngineSubsystemManager_ShutDownAll(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	for ( const SubsystemInitAndShutdown* subsystem = g_Subsystems + RAYGE_ARRAY_SIZE(g_Subsystems) - 1;
		  subsystem >= g_Subsystems;
		  --subsystem )
	{
		if ( subsystem->ShutDown )
		{
			subsystem->ShutDown();
		}
	}

	g_Initialised = false;
}
