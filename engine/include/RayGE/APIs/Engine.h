// File: Engine.h
// Defines overall interface to the game engine.

#pragma once

#include <stdint.h>
#include "RayGE/Export.h"
#include "RayGE/APIs/Logging.h"
#include "RayGE/APIs/Scene.h"
#include "RayGE/APIs/Game.h"
#include "RayGE/APIs/Renderable.h"

#define RAYGE_ENGINEAPI_VERSION_1 1

typedef struct RayGE_Engine_API_V1
{
	RayGE_Log_API log;
	RayGE_Scene_API scene;
	RayGE_Renderable_API renderable;
} RayGE_Engine_API_V1;

typedef struct RayGE_GameLib_Callbacks_V1
{
	RayGE_Game_Callbacks game;
	RayGE_Scene_Callbacks scene;
} RayGE_GameLib_Callbacks_V1;

typedef const RayGE_Engine_API_V1*(RAYGE_ENGINE_CDECL* RayGE_Engine_GetAPIFunc)(
	uint16_t /*requestedVersion*/,
	const RayGE_GameLib_Callbacks_V1* /*callbacks*/,
	uint16_t* /*outSupportedVersion*/
);
