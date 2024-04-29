#include <stdbool.h>
#include <stddef.h>
#include "RayGE/Private/CurrentEngineAPI.h"
#include "Subsystems/LoggingSubsystem.h"

const RayGE_Engine_API_Current g_EngineAPI = {
	// Logging
	{
		LoggingSubsystem_PrintLine,
	}
};

RayGE_Game_Callbacks_V1 g_GameCallbacks;

const RayGE_Engine_API_V1* RAYGE_ENGINE_CDECL EngineAPI_ExchangeAPIsWithGame(
	uint16_t requestedVersion,
	const RayGE_Game_Callbacks_V1* callbacks,
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

	g_GameCallbacks = *callbacks;
	return &g_EngineAPI;
}
