#include <stdio.h>
#include <stdlib.h>
#include "RayGE/Private/Launcher.h"
#include "Logging/Logging.h"
#include "Launcher/LaunchParams.h"
#include "Debugging.h"
#include "raylib.h"
#include "wzl_cutl/string.h"

#ifndef RAYGE_STATIC_LOG_BUFFER_SIZE
#define RAYGE_STATIC_LOG_BUFFER_SIZE 4096
#endif

#define LOG_BUFFER_SHIFT_SIZE (2 * LOG_MESSAGE_MAX_LENGTH)
#define TOTAL_LOG_BUFFER_SIZE (RAYGE_STATIC_LOG_BUFFER_SIZE + LOG_BUFFER_SHIFT_SIZE)

typedef struct LogBuffer
{
	char* cursor;
	char buffer[TOTAL_LOG_BUFFER_SIZE];
} LogBuffer;

static RayGE_Log_Level g_LogLevel = RAYGE_LOG_NONE;
static LogBuffer g_LogBuffer;
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

// Does not include the null terminator.
static size_t LogBufferBytesUsed(const LogBuffer* buffer)
{
	return (size_t)(buffer->cursor - buffer->buffer);
}

// Includes space for the null terminator.
static size_t LogBufferBytesLeft(const LogBuffer* buffer)
{
	return (size_t)((buffer->buffer + sizeof(buffer->buffer)) - buffer->cursor);
}

static void ResetBuffer(LogBuffer* buffer)
{
	buffer->cursor = buffer->buffer;
	(*buffer->cursor) = '\0';
}

static void ShiftLogBufferIfRequired(LogBuffer* buffer)
{
	const size_t bytesLeft = LogBufferBytesLeft(buffer);

	// Only perform the shift if we cannot definitely
	// fit another log message in.
	if ( bytesLeft >= LOG_MESSAGE_MAX_LENGTH )
	{
		// Don't have to do anything yet.
		return;
	}

	size_t contentLength = LogBufferBytesUsed(buffer);

	// We will shift everything back by LOG_BUFFER_SHIFT_SIZE.
	// If this would wipe out everything in the buffer, we can just
	// exit early here.
	if ( contentLength <= LOG_BUFFER_SHIFT_SIZE )
	{
		ResetBuffer(buffer);
		return;
	}

	contentLength -= LOG_BUFFER_SHIFT_SIZE;

	// Copy into the base of the buffer, beginning from LOG_BUFFER_SHIFT_SIZE
	// away from the base. We have calculated the remaining length of the
	// content that will be shifted down.
	memmove(buffer->buffer, buffer->buffer + LOG_BUFFER_SHIFT_SIZE, contentLength);

	// Make sure that the cursor now points to the correct place,
	// and that the new string is terminated.
	buffer->cursor = buffer->buffer + contentLength;
	(*buffer->cursor) = '\0';
}

static size_t AppendToLogBufferV(LogBuffer* buffer, size_t maxSpace, const char* format, va_list args)
{
	if ( maxSpace < 1 )
	{
		return 0;
	}

	int result = wzl_vsprintf(buffer->cursor, maxSpace, format, args);

	if ( result <= 0 )
	{
		return 0;
	}

	buffer->cursor += result;
	return (size_t)result;
}

static size_t AppendToLogBuffer(LogBuffer* buffer, size_t maxSpace, const char* format, ...)
{
	va_list args;

	va_start(args, format);
	size_t result = AppendToLogBufferV(buffer, maxSpace, format, args);
	va_end(args);

	return result;
}

static void PrintLogMessageLine(RayGE_Log_Level level, const char* source, const char* format, va_list args)
{
	if ( level < g_LogLevel )
	{
		return;
	}

	// After this call, we will have at least LOG_MESSAGE_MAX_LENGTH bytes to write into.
	ShiftLogBufferIfRequired(&g_LogBuffer);

	size_t bytesLeft = LOG_MESSAGE_MAX_LENGTH;
	const char* begin = g_LogBuffer.cursor;

	bytesLeft -= AppendToLogBuffer(&g_LogBuffer, bytesLeft, "%s", LogPrefix(level));

	if ( source && *source )
	{
		bytesLeft -= AppendToLogBuffer(&g_LogBuffer, bytesLeft, "[%s] ", source);
		printf("[%s] ", source);
	}

	bytesLeft -= AppendToLogBufferV(&g_LogBuffer, bytesLeft, format, args);
	bytesLeft -= AppendToLogBuffer(&g_LogBuffer, bytesLeft, "\n");

	printf("%s", begin);

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
	if ( g_Initialised )
	{
		return;
	}

	Logging_SetBackendDebugLogsEnabled(LaunchParams_GetLaunchState()->enableBackendDebugLogs);
	SetTraceLogCallback(&RaylibLogCallback);

	g_LogLevel = LaunchParams_GetLaunchState()->defaultLogLevel;
	ResetBuffer(&g_LogBuffer);

	g_Initialised = true;
}

void Logging_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	g_LogLevel = RAYGE_LOG_NONE;

	SetTraceLogLevel(LOG_NONE);
	SetTraceLogCallback(NULL);

	ResetBuffer(&g_LogBuffer);

	g_Initialised = false;
}

void Logging_SetLogLevel(RayGE_Log_Level level)
{
	if ( !g_Initialised )
	{
		return;
	}

	g_LogLevel = level;
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

	return g_LogBuffer.buffer;
}

size_t Logging_GetLogBufferTotalMessageLength(void)
{
	if ( !g_Initialised )
	{
		return 0;
	}

	return LogBufferBytesUsed(&g_LogBuffer);
}
