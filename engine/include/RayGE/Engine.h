// File: Engine.h
// Defines overall interface to the game engine.

#pragma once

#include <stdint.h>
#include "RayGE/Export.h"
#include "RayGE/Logging.h"

#define RAYGE_ENGINEAPI_VERSION_1 1

typedef struct RayGE_Engine_API_V1
{
	RayGE_Log_API log;
} RayGE_Engine_API_V1;

typedef struct RayGE_Game_Callbacks_V1
{
	void (*Game_StartUp)(void);
	void (*Game_ShutDown)(void);
} RayGE_Game_Callbacks_V1;

typedef const RayGE_Engine_API_V1*(RAYGE_ENGINE_CDECL* RayGE_Engine_GetAPIFunc)(
	uint16_t /*requestedVersion*/,
	const RayGE_Game_Callbacks_V1* /*callbacks*/,
	uint16_t* /*outSupportedVersion*/
);
