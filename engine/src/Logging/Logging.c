#include <stdio.h>
#include <stdlib.h>
#include "RayGE/Private/Launcher.h"
#include "Logging/Logging.h"
#include "Launcher/LaunchParams.h"
#include "Debugging.h"
#include "raylib.h"
#include "wzl_cutl/string.h"

static RayGE_Log_Level g_LogLevel = RAYGE_LOG_NONE;

static const char* LogPrefix(RayGE_Log_Level level)
{
	switch ( level )
	{
		case RAYGE_LOG_TRACE:
		{
			return "{Trace} ";
		}

		case RAYGE_LOG_WARNING:
		{
			return "Warning: ";
		}

		case RAYGE_LOG_ERROR:
		{
			return "Error: ";
		}

		case RAYGE_LOG_FATAL:
		{
			return "FATAL ERROR: ";
		}

		default:
		{
			return "";
		}
	}
}

static RayGE_Log_Level RaylibLogLevelToRayGELogLevel(int inLevel)
{
	switch ( inLevel )
	{
		case LOG_TRACE:
		{
			return RAYGE_LOG_TRACE;
		}

		case LOG_DEBUG:
		{
			return RAYGE_LOG_DEBUG;
		}

		case LOG_INFO:
		{
			return RAYGE_LOG_INFO;
		}

		case LOG_WARNING:
		{
			return RAYGE_LOG_WARNING;
		}

		case LOG_ERROR:
		{
			return RAYGE_LOG_ERROR;
		}

		case LOG_FATAL:
		{
			return RAYGE_LOG_FATAL;
		}

		default:
		{
			return RAYGE_LOG_INFO;
		}
	}
}

static void PrintLogMessageLine(RayGE_Log_Level level, const char* source, const char* format, va_list args)
{
	if ( level < g_LogLevel )
	{
		return;
	}

	printf("%s", LogPrefix(level));

	if ( source && *source )
	{
		printf("[%s] ", source);
	}

	vprintf(format, args);
	printf("\n");

	if ( level == RAYGE_LOG_FATAL )
	{
		RayGE_DebugBreak();

		// This isn't ideal given this is a library,
		// but unsure what else we can do. Receiving
		// a fatal log message implies that the engine
		// cannot progress any further, so we just quit.
		exit(RAYGE_LAUNCHER_EXIT_LOG_FATAL_ERROR);
	}
}

static void RaylibLogCallback(int logLevel, const char* format, va_list args)
{
	PrintLogMessageLine(RaylibLogLevelToRayGELogLevel(logLevel), "Raylib", format, args);
}

void Logging_Init(void)
{
	Logging_SetBackendDebugLogsEnabled(LaunchParams_GetLaunchState()->enableBackendDebugLogs);
	SetTraceLogCallback(&RaylibLogCallback);

	g_LogLevel = LaunchParams_GetLaunchState()->defaultLogLevel;
}

void Logging_ShutDown(void)
{
	g_LogLevel = RAYGE_LOG_NONE;

	SetTraceLogLevel(LOG_NONE);
	SetTraceLogCallback(NULL);
}

void Logging_SetLogLevel(RayGE_Log_Level level)
{
	g_LogLevel = level;
}

void Logging_SetBackendDebugLogsEnabled(bool enabled)
{
	SetTraceLogLevel(enabled ? LOG_DEBUG : LOG_NONE);
}

void Logging_PrintLineV(RayGE_Log_Level level, const char* format, va_list args)
{
	PrintLogMessageLine(level, NULL, format, args);
}
