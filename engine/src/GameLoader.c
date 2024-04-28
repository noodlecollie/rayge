#include <stddef.h>
#include <stdio.h>
#include "GameLoader.h"
#include "RayGE/Engine.h"
#include "RayGE/Private/CurrentEngineAPI.h"
#include "Subsystems/FileSubsystem.h"
#include "Subsystems/LoggingSubsystem.h"
#include "EngineAPI.h"
#include "wzl_cutl/libloader.h"
#include "wzl_cutl/string.h"
#include "cJSON/cJSON.h"

#define GAME_JSON_KEY_CLIENT_LIB "client_library"

static const RayGE_Engine_API_V1* RAYGE_ENGINE_CDECL
GetEngineAPI(uint16_t requestedVersion, uint16_t* outSupportedVersion)
{
	// Always pass back the version we support, if we can.
	if ( outSupportedVersion )
	{
		*outSupportedVersion = RAYGE_ENGINEAPI_VERSION_CURRENT;
	}

	if ( requestedVersion == RAYGE_ENGINEAPI_VERSION_CURRENT )
	{
		return &g_EngineAPI;
	}

	return NULL;
}

bool GameLoader_InvokeGameLibraryStartup(void* gameLibrary)
{
	if ( !gameLibrary )
	{
		return false;
	}

	GameLibFunc_Startup startupFunc =
		(GameLibFunc_Startup)wzl_get_library_function(gameLibrary, RAYGE_GAMELIBRARY_STARTUP_SYMBOL_NAME);

	if ( !startupFunc )
	{
		return false;
	}

	startupFunc(&GetEngineAPI);
	return true;
}

void GameLoader_InvokeGameLibraryShutdown(void* gameLibrary)
{
	if ( !gameLibrary )
	{
		return;
	}

	GameLibFunc_ShutDown shutdownFunc =
		(GameLibFunc_ShutDown)wzl_get_library_function(gameLibrary, RAYGE_GAMELIBRARY_SHUTDOWN_SYMBOL_NAME);

	if ( !shutdownFunc )
	{
		return;
	}

	shutdownFunc();
}

static cJSON* ParseJSONFromFile(const char* path)
{
	size_t size = 0;
	uint8_t* fileData = FileSubsystem_LoadFileData(path, &size);

	if ( !fileData )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Could not load game JSON from %s", path);
		return NULL;
	}

	cJSON* out = cJSON_ParseWithLength((const char*)fileData, size);
	FileSubsystem_UnloadFileData(fileData);

	if ( !out )
	{
		const char* errorPtr = cJSON_GetErrorPtr();
		LoggingSubsystem_PrintLine(
			RAYGE_LOG_ERROR,
			"Failed to parse %s. Error: %s",
			path,
			errorPtr ? errorPtr : "Unknown error"
		);

		return NULL;
	}

	return out;
}

static const char* GetGameClientLibraryStringFromJSON(cJSON* json)
{
	if ( !json || json->type != cJSON_Object )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_WARNING, "Game JSON root was not an object");
		return NULL;
	}

	cJSON* libItem = cJSON_GetObjectItem(json, GAME_JSON_KEY_CLIENT_LIB);

	if ( !libItem || libItem->type != cJSON_String )
	{
		LoggingSubsystem_PrintLine(
			RAYGE_LOG_WARNING,
			"Could not find valid \"%s\" property in game JSON",
			GAME_JSON_KEY_CLIENT_LIB
		);

		return NULL;
	}

	return libItem->valuestring;
}

void* GameLoader_LoadLibraryFromDirectory(const char* dirPath)
{
	FileSubsystem_Path gameJsonPath;
	wzl_sprintf(gameJsonPath, sizeof(gameJsonPath), "%s/game.json", dirPath);

	cJSON* json = ParseJSONFromFile(gameJsonPath);

	if ( !json )
	{
		// Error will have been logged.
		return NULL;
	}

	void* libHandle = NULL;

	do
	{
		const char* libName = GetGameClientLibraryStringFromJSON(json);

		if ( !libName )
		{
			// Error will have been logged.
			break;
		}

		// TODO: Load library
	}
	while ( false );

	cJSON_Delete(json);
	return libHandle;
}

void GameLoader_UnloadLibrary(void* handle)
{
	if ( handle )
	{
		wzl_unload_library(handle);
	}
}
