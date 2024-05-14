#include <stdbool.h>
#include "Subsystems/SubsystemManager.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Subsystems/RendererSubsystem.h"
#include "Subsystems/UISubsystem.h"
#include "Subsystems/InputSubsystem.h"
#include "Subsystems/CommandSubsystem.h"
#include "Subsystems/InputHookSubsystem.h"
#include "Utils.h"

typedef struct SubsystemInitAndShutdown
{
	void (*Init)(void);
	void (*ShutDown)(void);
} SubsystemInitAndShutdown;

// The following exclude the logging subsystem, since this is handled independently.
// Subsystems are initialised in order, and shut down in reverse order.
static const SubsystemInitAndShutdown g_Subsystems[] = {
	{MemPoolSubsystem_Init, MemPoolSubsystem_ShutDown},
	// TODO: File subsystem here, and make it use its own mempool
	{RendererSubsystem_Init, RendererSubsystem_ShutDown},
	{InputSubsystem_Init, InputSubsystem_ShutDown},
	{UISubsystem_Init, UISubsystem_ShutDown},
	{CommandSubsystem_Init, CommandSubsystem_ShutDown},
	{InputHookSubsystem_Init, InputHookSubsystem_ShutDown},
};

static bool g_Initialised = false;

void SubsystemManager_InitAll(void)
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

void SubsystemManager_ShutDownAll(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	const SubsystemInitAndShutdown* end = g_Subsystems - 1;

	for ( const SubsystemInitAndShutdown* subsystem = g_Subsystems + RAYGE_ARRAY_SIZE(g_Subsystems) - 1; subsystem > end;
		  --subsystem )
	{
		if ( subsystem->ShutDown )
		{
			subsystem->ShutDown();
		}
	}

	g_Initialised = false;
}
