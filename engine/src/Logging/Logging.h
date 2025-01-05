#pragma once

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include "RayGE/APIs/Logging.h"

#define LOG_MESSAGE_MAX_LENGTH 512

typedef void (*Logging_Callback)(
	RayGE_Log_Level /* level */,
	const char* /* message */,
	size_t /* length */,
	void* /* userData */
);

void Logging_Init(void);
void Logging_ShutDown(void);
void Logging_SetLogLevel(RayGE_Log_Level level);
void Logging_SetBackendDebugLogsEnabled(bool enabled);
void Logging_PrintLineV(RayGE_Log_Level level, const char* format, va_list args);

// Must be initialised beforehand.
// On shutdown, all listeners are unregistered.
void Logging_AddListener(Logging_Callback callback, void* userData);

static inline void Logging_PrintLine(RayGE_Log_Level level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Logging_PrintLineV(level, format, args);
	va_end(args);
}
