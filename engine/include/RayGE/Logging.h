// File: Logging.h
// Interface for logging messages from the game engine.

#pragma once

typedef enum RayGE_Log_Level
{
	RAYGE_LOG_ALL = 0,
	RAYGE_LOG_TRACE,
	RAYGE_LOG_DEBUG,
	RAYGE_LOG_INFO,
	RAYGE_LOG_WARNING,
	RAYGE_LOG_ERROR,
	RAYGE_LOG_FATAL,
	RAYGE_LOG_NONE,
} RayGE_Log_Level;

typedef struct RayGE_Log_API
{
	void (*logMessage)(RayGE_Log_Level level, const char* format, ...);
} RayGE_Log_API;
