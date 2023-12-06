#include <stdio.h>
#include "Subsystems/LoggingSubsystem.h"
#include "raylib.h"

#define MAX_LOG_MESSAGE_LENGTH 256

static void LogCallback(int logLevel, const char *text, va_list args)
{
	// TODO: Make this more sophisticated.
	(void)logLevel;
	vprintf(text, args);
}

void LoggingSubsystem_Init(void)
{
	SetTraceLogCallback(LogCallback);
}

void LoggingSubsystem_ShutDown(void)
{
	SetTraceLogCallback(NULL);
}

void LoggingSubsystem_EmitMessageV(RayGE_Log_Level level, const char* format, va_list args)
{
	char message[MAX_LOG_MESSAGE_LENGTH];
	vsprintf_s(message, sizeof(message), format, args);

	// TODO: We may want to have some more sophisticated handling
	// of when a fatal error occurs. Perhaps have systems register
	// cleanup functions which have to run before the library exits?
	// Check to see what Xash does with its mempools.
	TraceLog(level, "%s", message);
}
