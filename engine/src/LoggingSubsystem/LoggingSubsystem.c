#include <stdio.h>
#include "LoggingSubsystem/LoggingSubsystem.h"
#include "raylib.h"

#define MAX_LOG_MESSAGE_LENGTH 256

void LoggingSubsystem_EmitMessageV(RayGE_Log_Level level, const char* format, va_list args)
{
	char message[MAX_LOG_MESSAGE_LENGTH];
	vsprintf_s(message, sizeof(message), format, args);
	TraceLog(level, "%s", message);
}
