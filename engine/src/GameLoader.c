#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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
		LoggingSubsystem_PrintLine(
			RAYGE_LOG_ERROR,
			"Could not resolve " RAYGE_GAMELIBRARY_STARTUP_SYMBOL_NAME " function in game library."
		);

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
		LoggingSubsystem_PrintLine(
			RAYGE_LOG_ERROR,
			"Could not resolve " RAYGE_GAMELIBRARY_SHUTDOWN_SYMBOL_NAME " function in game library."
		);

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

	if ( !out )
	{
		// cJSON is not the best at reporting errors,
		// but not sure what else we can really do here...
		const char* errorPtr = cJSON_GetErrorPtr();

		LoggingSubsystem_PrintLine(
			RAYGE_LOG_ERROR,
			"Failed to parse %s. Parse failed at character %zu",
			path,
			errorPtr - (const char*)fileData
		);
	}

	FileSubsystem_UnloadFileData(fileData);
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
			"Could not find valid \"" GAME_JSON_KEY_CLIENT_LIB "\" property in game JSON"
		);

		return NULL;
	}

	return cJSON_GetStringValue(libItem);
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

		LoggingSubsystem_PrintLine(RAYGE_LOG_DEBUG, "Loading game client library: %s", libName);

		FileSubsystem_Path libPathRelativeToJson;
		wzl_sprintf(libPathRelativeToJson, sizeof(libPathRelativeToJson), "%s/%s", dirPath, libName);

		char* absPath = (char*)malloc(FILESYSTEM_MAX_ABS_PATH);

		FileSubsystem_MakeAbsolute(libPathRelativeToJson, absPath, FILESYSTEM_MAX_ABS_PATH);
		LoggingSubsystem_PrintLine(RAYGE_LOG_TRACE, "Absolute path to game client library: %s", absPath);

		libHandle = wzl_load_library(absPath);

		free(absPath);

		if ( !libHandle )
		{
			LoggingSubsystem_PrintLine(
				RAYGE_LOG_ERROR,
				"Loading %s failed. Message: %s",
				libName,
				wzl_get_last_library_error()
			);

			break;
		}

		LoggingSubsystem_PrintLine(RAYGE_LOG_TRACE, "Successfully loaded game client library from disk.");
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
