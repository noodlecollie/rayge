#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "RayGE/APIs/Engine.h"

// The "current" engine API is the API version that this engine
// build uses and supports. This should be incremented between
// different releases, if the public API has changed since
// the last release.
#define RAYGE_ENGINEAPI_VERSION_CURRENT RAYGE_ENGINEAPI_VERSION_1
typedef RayGE_Engine_API_V1 RayGE_Engine_API_Current;
typedef RayGE_GameLib_Callbacks_V1 RayGE_GameLib_Callbacks_Current;

extern const RayGE_Engine_API_Current g_EngineAPI;
extern RayGE_GameLib_Callbacks_Current g_GameLibCallbacks;

const RayGE_Engine_API_Current* RAYGE_ENGINE_CDECL EngineAPI_ExchangeAPIsWithGame(
	uint16_t requestedVersion,
	const RayGE_GameLib_Callbacks_Current* callbacks,
	uint16_t* outSupportedVersion
);

// Helper macro for calling callbacks only if they've been set
#define INVOKE_CALLBACK_V(cb, ...) \
	do \
	{ \
		if ( (cb) ) \
		{ \
			(cb)(__VA_ARGS__); \
		} \
	} \
	while ( false )

#define INVOKE_CALLBACK(cb) \
	do \
	{ \
		if ( (cb) ) \
		{ \
			(cb)(); \
		} \
	} \
	while ( false )
