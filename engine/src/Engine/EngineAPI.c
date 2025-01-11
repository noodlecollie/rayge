#include <stdbool.h>
#include <stddef.h>
#include "Engine/EngineAPI.h"
#include "Logging/Logging.h"
#include "Scene/SceneAPI.h"
#include "Rendering/RenderableAPI.h"

const RayGE_Engine_API_Current g_EngineAPI = {
	// Logging
	{
		Logging_PrintLine,
	},

	// Scene
	{
		SceneAPI_CreateEntity,
		SceneAPI_AddSpatialComponent,
		SceneAPI_GetSpatialComponent,
		SceneAPI_AddCameraComponent,
		SceneAPI_GetCameraComponent,
		SceneAPI_AddRenderableComponent,
		SceneAPI_GetRenderableComponent,
	},

	// Renderable
	{
		RenderableAPI_GetPrimitiveHandle,
		RenderableAPI_LoadTexture,
		RenderableAPI_UnloadTexture,
	}
};

RayGE_GameLib_Callbacks_Current g_GameLibCallbacks;

const RayGE_Engine_API_Current* RAYGE_ENGINE_CDECL EngineAPI_ExchangeAPIsWithGame(
	uint16_t requestedVersion,
	const RayGE_GameLib_Callbacks_Current* callbacks,
	uint16_t* outSupportedVersion
)
{
	// Always pass back the version we support, if we can.
	if ( outSupportedVersion )
	{
		*outSupportedVersion = RAYGE_ENGINEAPI_VERSION_CURRENT;
	}

	if ( requestedVersion != RAYGE_ENGINEAPI_VERSION_CURRENT || !callbacks )
	{
		return NULL;
	}

	g_GameLibCallbacks = *callbacks;
	return &g_EngineAPI;
}
