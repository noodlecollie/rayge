#pragma once

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include "RayGE/APIs/Logging.h"

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

// This is the max length a printable string can be,
// and the maximum value that may be returned
// by Logging_GetLogBufferTotalMessageLength(). It
// does not include the null terminator.
size_t Logging_GetLogBufferMaxMessageLength(void);

// Returns the value of the log counter, which is incremented each time
// a message is logged. This is useful for knowing when the log buffer
// contents have changed.
size_t Logging_GetLogCounter(void);

static inline void Logging_PrintLine(RayGE_Log_Level level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Logging_PrintLineV(level, format, args);
	va_end(args);
}
