#pragma once

#include <stdbool.h>
#include <stdarg.h>
#include "RayGE/APIs/Logging.h"

void Logging_Init(void);
void Logging_ShutDown(void);
void Logging_SetLogLevel(RayGE_Log_Level level);
void Logging_SetBackendDebugLogsEnabled(bool enabled);
void Logging_PrintLineV(RayGE_Log_Level level, const char* format, va_list args);

static inline void Logging_PrintLine(RayGE_Log_Level level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Logging_PrintLineV(level, format, args);
	va_end(args);
}
