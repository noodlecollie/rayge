#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "RayGE/APIs/Logging.h"
#include "RayGE/Private/Launcher.h"
#include "Logging/Logging.h"
#include "Logging/LogBackingBuffer.h"
#include "Launcher/LaunchParams.h"
#include "Debugging.h"
#include "raylib.h"
#include "wzl_cutl/string.h"
#include "wzl_cutl/time.h"

#ifndef RAYGE_LOG_BUFFER_SIZE
#define RAYGE_LOG_BUFFER_SIZE 4096
#endif

typedef struct LogData
{
	RayGE_Log_Level logLevel;
	LogBackingBuffer* backingBuffer;
	size_t messageCounter;
	bool printing;
} LogData;

static LogData g_LogData;
static bool g_Initialised = false;

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

static size_t AppendToLogBufferV(char** buffer, size_t* bufferSize, const char* format, va_list args)
{
	if ( *bufferSize < 1 )
	{
		return 0;
	}

	int result = wzl_vsprintf(*buffer, *bufferSize, format, args);

	if ( result <= 0 )
	{
		return 0;
	}

	size_t sResult = (size_t)result;

	RAYGE_ENSURE(
		sResult < *bufferSize,
		"Wrote more characters than there was space in the buffer - this should not happen!"
	);

	*buffer += sResult;
	*bufferSize -= sResult;
	return sResult;
}

static size_t AppendToLogBuffer(char** buffer, size_t* bufferSize, const char* format, ...)
{
	va_list args;

	va_start(args, format);
	size_t result = AppendToLogBufferV(buffer, bufferSize, format, args);
	va_end(args);

	return result;
}

static void PrintLogMessageLine(RayGE_Log_Level level, const char* source, const char* format, va_list args)
{
	if ( level < g_LogData.logLevel )
	{
		return;
	}

	if ( g_LogData.printing )
	{
		// Something weird has happened, and we've ended up re-entering this function.
		// Make sure we know about it, and just exit - this should never happen.
		printf("PrintLogMessageLine: Unexpected re-entry!\n");
		RayGE_DebugBreak();
		exit(RAYGE_LAUNCHER_EXIT_LOG_FATAL_ERROR);
	}

	g_LogData.printing = true;

	char messageBuffer[LOG_BUFFER_MESSAGE_MAX_LENGTH];
	char* cursor = messageBuffer;
	size_t bytesLeft = sizeof(messageBuffer);

	AppendToLogBuffer(&cursor, &bytesLeft, "%s", LogPrefix(level));

	if ( source && *source )
	{
		AppendToLogBuffer(&cursor, &bytesLeft, "[%s] ", source);
	}

	AppendToLogBufferV(&cursor, &bytesLeft, format, args);
	AppendToLogBuffer(&cursor, &bytesLeft, "\n");

	printf("%s", messageBuffer);
	LogBackingBuffer_Append(g_LogData.backingBuffer, messageBuffer, sizeof(messageBuffer) - bytesLeft);
	++g_LogData.messageCounter;

	if ( level == RAYGE_LOG_FATAL )
	{
		RayGE_DebugBreak();

		// This isn't ideal given this is a library,
		// but unsure what else we can do. Receiving
		// a fatal log message implies that the engine
		// cannot progress any further, so we just quit.
		exit(RAYGE_LAUNCHER_EXIT_LOG_FATAL_ERROR);
	}

	g_LogData.printing = false;
}

static void RaylibLogCallback(int logLevel, const char* format, va_list args)
{
	PrintLogMessageLine(RaylibLogLevelToRayGELogLevel(logLevel), "Raylib", format, args);
}

void Logging_Init(void)
{
	if ( g_Initialised )
	{
		return;
	}

	Logging_SetBackendDebugLogsEnabled(LaunchParams_GetLaunchState()->enableBackendDebugLogs);
	SetTraceLogCallback(&RaylibLogCallback);

	g_LogData.logLevel = LaunchParams_GetLaunchState()->defaultLogLevel;
	g_LogData.backingBuffer = LogBackingBuffer_Create(RAYGE_LOG_BUFFER_SIZE);
	g_LogData.messageCounter = 0;

	g_Initialised = true;
}

void Logging_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	g_Initialised = false;
	g_LogData.logLevel = RAYGE_LOG_NONE;
	g_LogData.messageCounter = 0;

	SetTraceLogLevel(LOG_NONE);
	SetTraceLogCallback(NULL);

	LogBackingBuffer_Destroy(g_LogData.backingBuffer);
	g_LogData.backingBuffer = NULL;
}

void Logging_SetLogLevel(RayGE_Log_Level level)
{
	if ( !g_Initialised )
	{
		return;
	}

	g_LogData.logLevel = level;
}

void Logging_SetBackendDebugLogsEnabled(bool enabled)
{
	if ( !g_Initialised )
	{
		return;
	}

	SetTraceLogLevel(enabled ? LOG_DEBUG : LOG_NONE);
}

void Logging_PrintLineV(RayGE_Log_Level level, const char* format, va_list args)
{
	if ( !g_Initialised )
	{
		return;
	}

	PrintLogMessageLine(level, NULL, format, args);
}

const char* Logging_GetLogBufferBase(void)
{
	if ( !g_Initialised )
	{
		return NULL;
	}

	return LogBackingBuffer_Begin(g_LogData.backingBuffer);
}

size_t Logging_GetLogBufferTotalMessageLength(void)
{
	if ( !g_Initialised )
	{
		return 0;
	}

	return LogBackingBuffer_StringLength(g_LogData.backingBuffer);
}

size_t Logging_GetLogBufferMaxMessageLength(void)
{
	if ( !g_Initialised )
	{
		return 0;
	}

	return LogBackingBuffer_MaxStringLength(g_LogData.backingBuffer);
}

size_t Logging_GetLogCounter(void)
{
	if ( !g_Initialised )
	{
		return 0;
	}

	return g_LogData.messageCounter;
}

size_t Logging_GetBufferTotalSize(void)
{
	if ( !g_Initialised )
	{
		return 0;
	}

	return RAYGE_LOG_BUFFER_SIZE;
}
