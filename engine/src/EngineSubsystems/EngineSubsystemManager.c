#include <stdbool.h>
#include "EngineSubsystems/EngineSubsystemManager.h"
#include "EngineSubsystems/FilesystemSubsystem.h"
#include "EngineSubsystems/InputSubsystem.h"
#include "EngineSubsystems/CommandSubsystem.h"
#include "EngineSubsystems/InputHookSubsystem.h"
#include "EngineSubsystems/SceneSubsystem.h"
#include "EngineSubsystems/ResourceSubsystem.h"
#include "BehaviouralSubsystems/BSysManager.h"
#include "Utils/Utils.h"
#include "Headless.h"

#if !RAYGE_HEADLESS()
#include "EngineSubsystems/RendererSubsystem.h"
#include "EngineSubsystems/UISubsystem.h"
#endif

typedef struct SubsystemInitAndShutdown
{
	void (*Init)(void);
	void (*ShutDown)(void);
} SubsystemInitAndShutdown;

// Initialised in order, shut down in reverse order
static const SubsystemInitAndShutdown g_Subsystems[] = {
	{FilesystemSubsystem_Init, FilesystemSubsystem_ShutDown},

#if !RAYGE_HEADLESS()
	{RendererSubsystem_Init, RendererSubsystem_ShutDown},
#endif

	{ResourceSubsystem_Init, ResourceSubsystem_ShutDown},
	{SceneSubsystem_Init, SceneSubsystem_ShutDown},
	{InputSubsystem_Init, InputSubsystem_ShutDown},
	{InputHookSubsystem_Init, InputHookSubsystem_ShutDown},

#if !RAYGE_HEADLESS()
	{UISubsystem_Init, UISubsystem_ShutDown},
#endif

	{CommandSubsystem_Init, CommandSubsystem_ShutDown},
	{BSysManager_Init, BSysManager_ShutDown},
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
