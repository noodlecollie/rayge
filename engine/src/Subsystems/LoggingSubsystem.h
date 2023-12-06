#pragma once

#include <stdarg.h>
#include "RayGE/Logging.h"

void LoggingSubsystem_Init(void);
void LoggingSubsystem_ShutDown(void);
void LoggingSubsystem_EmitMessageV(RayGE_Log_Level level, const char* format, va_list args);

static inline void LoggingSubsystem_EmitMessage(RayGE_Log_Level level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LoggingSubsystem_EmitMessageV(level, format, args);
	va_end(args);
}
