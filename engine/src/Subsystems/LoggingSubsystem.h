#pragma once

#include <stdbool.h>
#include <stdarg.h>
#include "RayGE/Logging.h"

void LoggingSubsystem_Init(void);
void LoggingSubsystem_ShutDown(void);
void LoggingSubsystem_SetLogLevel(RayGE_Log_Level level);
void LoggingSubsystem_SetBackendDebugLogsEnabled(bool enabled);
void LoggingSubsystem_PrintLineV(RayGE_Log_Level level, const char* format, va_list args);

static inline void LoggingSubsystem_PrintLine(RayGE_Log_Level level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LoggingSubsystem_PrintLineV(level, format, args);
	va_end(args);
}
