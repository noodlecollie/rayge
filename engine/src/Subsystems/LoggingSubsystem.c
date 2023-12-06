#include <stdio.h>
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

void LoggingSubsystem_EmitMessageV(RayGE_Log_Level level, const char* format, va_list args)
{
	if ( level < g_LogLevel )
	{
		return;
	}

	// TODO: Make this more sophisticated.
	vprintf(format, args);
}
