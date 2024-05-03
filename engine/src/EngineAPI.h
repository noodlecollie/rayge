#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "RayGE/APIs/Engine.h"

extern const RayGE_Engine_API_V1 g_EngineAPI;
extern RayGE_GameLib_Callbacks_V1 g_GameLibCallbacks;

const RayGE_Engine_API_V1* RAYGE_ENGINE_CDECL EngineAPI_ExchangeAPIsWithGame(
	uint16_t requestedVersion,
	const RayGE_GameLib_Callbacks_V1* callbacks,
	uint16_t* outSupportedVersion
);

// Helper macro for calling callbacks only if they've been set
#define INVOKE_CALLBACK(cb, ...) \
	do \
	{ \
		if ( (cb) ) \
		{ \
			(cb)(__VA_ARGS__); \
		} \
	} \
	while ( false )
