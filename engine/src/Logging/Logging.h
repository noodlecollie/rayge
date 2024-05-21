#pragma once

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include "RayGE/APIs/Logging.h"

// Including the null terminator
#define LOG_MESSAGE_MAX_LENGTH 512

void Logging_Init(void);
void Logging_ShutDown(void);
void Logging_SetLogLevel(RayGE_Log_Level level);
void Logging_SetBackendDebugLogsEnabled(bool enabled);
void Logging_PrintLineV(RayGE_Log_Level level, const char* format, va_list args);

const char* Logging_GetLogBufferBase(void);

// This is the length of the total string in the buffer at
// the time of the call, not including the null terminator.
// This may no longer be accurate if something writes a new
// log message after this function is called!
// Use with caution.
size_t Logging_GetLogBufferTotalMessageLength(void);

static inline void Logging_PrintLine(RayGE_Log_Level level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Logging_PrintLineV(level, format, args);
	va_end(args);
}
