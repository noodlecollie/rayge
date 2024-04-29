#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "GameLoader.h"
#include "RayGE/Platform.h"
#include "RayGE/Engine.h"
#include "RayGE/Private/CurrentEngineAPI.h"
#include "Subsystems/FileSubsystem.h"
#include "Subsystems/LoggingSubsystem.h"
#include "EngineAPI.h"
#include "wzl_cutl/libloader.h"
#include "wzl_cutl/string.h"
#include "cJSON/cJSON.h"
#include "JSON/JSONUtils.h"

#define GAME_JSON_KEY_CLIENT_LIB "client_library"

#if RAYGE_PLATFORM() == RAYGE_PLATFORM_LINUX
#define GAME_JSON_KEY_PLATFORM_NAME "linux"
#elif RAYGE_PLATFORM() == RAYGE_PLATFORM_WINDOWS
#define GAME_JSON_KEY_PLATFORM_NAME "windows"
#endif

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
	if ( !json || !cJSON_IsObject(json) )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_WARNING, "Game JSON root was not an object");
		return NULL;
	}

	cJSON* libItem = JSONUtils_ExpectObjectItem("Game JSON", json, GAME_JSON_KEY_CLIENT_LIB);

	if ( !libItem )
	{
		return NULL;
	}

	cJSON* libPathItem = JSONUtils_ExpectStringItem("Game lib config", libItem, GAME_JSON_KEY_PLATFORM_NAME);
	return libPathItem ? cJSON_GetStringValue(libPathItem) : NULL;
}

static cJSON* LoadGameJSON(const char* dirPath)
{
	FileSubsystem_Path gameJsonPath;
	wzl_sprintf(gameJsonPath, sizeof(gameJsonPath), "%s/game.json", dirPath);

	// This will log an error if it fails
	return ParseJSONFromFile(gameJsonPath);
}

static char* ComputeGameLibraryAbsolutePath(const char* dirPath, cJSON* gameJSON)
{
	if ( !gameJSON )
	{
		return NULL;
	}

	const char* libName = GetGameClientLibraryStringFromJSON(gameJSON);

	if ( !libName )
	{
		// Error will have been logged.
		return NULL;
	}

	LoggingSubsystem_PrintLine(RAYGE_LOG_DEBUG, "Using game client library: %s", libName);

	FileSubsystem_Path libPathRelativeToJson;
	wzl_sprintf(libPathRelativeToJson, sizeof(libPathRelativeToJson), "%s/%s", dirPath, libName);

	char* absPath = (char*)malloc(FILESYSTEM_MAX_ABS_PATH);
	FileSubsystem_MakeAbsolute(libPathRelativeToJson, absPath, FILESYSTEM_MAX_ABS_PATH);

	LoggingSubsystem_PrintLine(RAYGE_LOG_TRACE, "Absolute path to game client library: %s", absPath);
	return absPath;
}

static void* LoadGameLibraryFromGameJSON(const char* dirPath)
{
	void* libHandle = NULL;
	cJSON* gameJSON = NULL;
	char* libAbsPath = NULL;

	do
	{
		gameJSON = LoadGameJSON(dirPath);

		if ( !gameJSON )
		{
			break;
		}

		libAbsPath = ComputeGameLibraryAbsolutePath(dirPath, gameJSON);

		if ( !libAbsPath )
		{
			break;
		}

		libHandle = wzl_load_library(libAbsPath);

		if ( !libHandle )
		{
			LoggingSubsystem_PrintLine(
				RAYGE_LOG_ERROR,
				"Loading %s failed. Message: %s",
				libAbsPath,
				wzl_get_last_library_error()
			);

			break;
		}

		LoggingSubsystem_PrintLine(RAYGE_LOG_TRACE, "Successfully loaded game client library from disk.");
	}
	while ( false );

	if ( gameJSON )
	{
		cJSON_Delete(gameJSON);
	}

	if ( libAbsPath )
	{
		free(libAbsPath);
	}

	return libHandle;
}

void* GameLoader_LoadLibraryFromDirectory(const char* dirPath)
{
	void* libHandle = LoadGameLibraryFromGameJSON(dirPath);

	if ( !libHandle )
	{
		return NULL;
	}

	GameLibFunc_ExchangeAPIs apiFunc =
		(GameLibFunc_ExchangeAPIs)wzl_get_library_function(libHandle, RAYGE_GAMELIBRARY_EXCHANGEAPIS_SYMBOL_NAME);

	if ( !apiFunc )
	{
		LoggingSubsystem_PrintLine(
			RAYGE_LOG_ERROR,
			"Could not resolve " RAYGE_GAMELIBRARY_EXCHANGEAPIS_SYMBOL_NAME " function in game library."
		);

		wzl_unload_library(libHandle);
		return NULL;
	}

	// TODO: Validation of game callbacks after this call.
	apiFunc(&EngineAPI_ExchangeAPIsWithGame);

	return libHandle;
}

void GameLoader_UnloadLibrary(void* handle)
{
	if ( handle )
	{
		wzl_unload_library(handle);
	}
}
