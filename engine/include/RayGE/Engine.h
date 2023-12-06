// File: Engine.h
// Defines overall interface to the game engine.

#pragma once

#include <stdint.h>
#include "RayGE/Export.h"
#include "RayGE/Logging.h"

#define RAYGE_ENGINEAPI_VERSION 1

typedef struct RayGE_Engine_API
{
	RayGE_Log_API logging;
} RayGE_Engine_API;

typedef const RayGE_Engine_API* (RAYGE_ENGINE_CDECL* RayGE_Engine_GetAPIFuncPtr)(int32_t /*version*/);
