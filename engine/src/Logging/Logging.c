#include <stdio.h>
#include <stdlib.h>
#include "RayGE/APIs/Logging.h"
#include "RayGE/Private/Launcher.h"
#include "Logging/Logging.h"
#include "Launcher/LaunchParams.h"
#include "MemPool//MemPoolManager.h"
#include "Debugging.h"
#include "raylib.h"
#include "wzl_cutl/string.h"
#include "utlist.h"

#ifndef RAYGE_LOG_BUFFER_SIZE
#define RAYGE_LOG_BUFFER_SIZE 4096
#endif

typedef struct Listener
{
	struct Listener* next;
	Logging_Callback callback;
	void* userData;
} Listener;

typedef struct LogData
{
	RayGE_Log_Level logLevel;
	bool printing;
	Listener* listeners;
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

static void SetBackendDebugLogsEnabled(bool enabled)
{
	SetTraceLogLevel(enabled ? LOG_DEBUG : LOG_NONE);
}

static void DeleteAllListeners(void)
{
	Listener* item = NULL;
	Listener* temp = NULL;

	LL_FOREACH_SAFE(g_LogData.listeners, item, temp)
	{
		LL_DELETE(g_LogData.listeners, item);
		MEMPOOL_FREE(item);
	}
}

static void EmitOnAllListeners(RayGE_Log_Level level, const char* message, size_t length)
{
	Listener* item = NULL;

	LL_FOREACH(g_LogData.listeners, item)
	{
		item->callback(level, message, length, item->userData);
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

	char messageBuffer[LOG_MESSAGE_MAX_LENGTH];
	char* cursor = messageBuffer;
	size_t bytesLeft = sizeof(messageBuffer);

	AppendToLogBuffer(&cursor, &bytesLeft, "%s", LogPrefix(level));

	if ( source && *source )
	{
		AppendToLogBuffer(&cursor, &bytesLeft, "[%s] ", source);
	}

	AppendToLogBufferV(&cursor, &bytesLeft, format, args);
	AppendToLogBuffer(&cursor, &bytesLeft, "\n");

	// bytesLeft is decremented by the number of characters that we wrote.
	// The difference between its current and original value is the length.
	const size_t messageLength = sizeof(messageBuffer) - bytesLeft;

	printf("%s", messageBuffer);
	EmitOnAllListeners(level, messageBuffer, messageLength);

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
	RAYGE_ASSERT_BREAK(!g_Initialised);

	if ( g_Initialised )
	{
		return;
	}

	SetBackendDebugLogsEnabled(LaunchParams_GetLaunchState()->enableBackendDebugLogs);
	SetTraceLogCallback(&RaylibLogCallback);

	g_LogData.logLevel = LaunchParams_GetLaunchState()->defaultLogLevel;

	g_Initialised = true;
}

void Logging_ShutDown(void)
{
	RAYGE_ASSERT_BREAK(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	RAYGE_ASSERT_BREAK(!g_LogData.printing);

	DeleteAllListeners();

	g_Initialised = false;
	g_LogData.logLevel = RAYGE_LOG_NONE;

	SetTraceLogLevel(LOG_NONE);
	SetTraceLogCallback(NULL);
}

void Logging_SetLogLevel(RayGE_Log_Level level)
{
	RAYGE_ASSERT_BREAK(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	RAYGE_ASSERT_BREAK(!g_LogData.printing);

	g_LogData.logLevel = level;
}

void Logging_SetBackendDebugLogsEnabled(bool enabled)
{
	RAYGE_ASSERT_BREAK(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	RAYGE_ASSERT_BREAK(!g_LogData.printing);

	SetBackendDebugLogsEnabled(enabled);
}

void Logging_PrintLineV(RayGE_Log_Level level, const char* format, va_list args)
{
	RAYGE_ASSERT_BREAK(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	RAYGE_ASSERT_BREAK(!g_LogData.printing);

	PrintLogMessageLine(level, NULL, format, args);
}

void Logging_AddListener(Logging_Callback callback, void* userData)
{
	RAYGE_ASSERT_BREAK(g_Initialised);
	RAYGE_ASSERT_BREAK(callback);

	if ( !g_Initialised || !callback )
	{
		return;
	}

	RAYGE_ASSERT_BREAK(!g_LogData.printing);

	Listener* listener = MEMPOOL_CALLOC_STRUCT(MEMPOOL_LOGGING, Listener);
	listener->callback = callback;
	listener->userData = userData;

	LL_APPEND(g_LogData.listeners, listener);
}

void Logging_RemoveListener(Logging_Callback callback)
{
	RAYGE_ASSERT_BREAK(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	Listener* item = NULL;
	Listener* temp = NULL;

	LL_FOREACH_SAFE(g_LogData.listeners, item, temp)
	{
		if ( item->callback == callback )
		{
			LL_DELETE(g_LogData.listeners, item);
			return;
		}
	}
}
