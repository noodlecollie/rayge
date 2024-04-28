#include <stdio.h>
#include <stdlib.h>
#include "RayGE/Private/Launcher.h"
#include "Subsystems/LoggingSubsystem.h"
#include "raylib.h"

static RayGE_Log_Level g_LogLevel = RAYGE_LOG_NONE;

void LoggingSubsystem_Init(void)
{
	// Disable raylib logging
	SetTraceLogLevel(LOG_NONE);

	g_LogLevel = RAYGE_LOG_INFO;
}

void LoggingSubsystem_ShutDown(void)
{
	g_LogLevel = RAYGE_LOG_NONE;
}

void LoggingSubsystem_SetLogLevel(RayGE_Log_Level level)
{
	g_LogLevel = level;
}

void LoggingSubsystem_PrintLineV(RayGE_Log_Level level, const char* format, va_list args)
{
	if ( level < g_LogLevel )
	{
		return;
	}

	// TODO: Make this more sophisticated.
	vprintf(format, args);
	printf("\n");

	if ( level == RAYGE_LOG_FATAL )
	{
		// This isn't ideal given this is a library,
		// but unsure what else we can do. Receiving
		// a fatal log message implies that the engine
		// cannot progress any further, so we just quit.
		exit(RAYGE_LAUNCHER_EXIT_LOG_FATAL_ERROR);
	}
}
