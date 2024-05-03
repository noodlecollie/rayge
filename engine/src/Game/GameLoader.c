#include <stdlib.h>
#include "GameLoader.h"
#include "Subsystems/FileSubsystem.h"
#include "Subsystems/LoggingSubsystem.h"
#include "EngineAPI.h"
#include "wzl_cutl/libloader.h"
#include "wzl_cutl/string.h"
#include "Game/GameData.h"

static char* ComputeGameLibraryAbsolutePath(const char* dirPath)
{
	const char* libPath = GameData_GetGameLibraryPath();
	LoggingSubsystem_PrintLine(RAYGE_LOG_DEBUG, "Using game client library: %s", libPath);

	FileSubsystem_Path libPathRelativeToJson;
	wzl_sprintf(libPathRelativeToJson, sizeof(libPathRelativeToJson), "%s/%s", dirPath, libPath);

	char* absPath = (char*)malloc(FILESYSTEM_MAX_ABS_PATH);
	FileSubsystem_MakeAbsolute(libPathRelativeToJson, absPath, FILESYSTEM_MAX_ABS_PATH);

	LoggingSubsystem_PrintLine(RAYGE_LOG_TRACE, "Absolute path to game client library: %s", absPath);
	return absPath;
}

static void* LoadGameLibraryFromGameJSON(const char* dirPath)
{
	void* libHandle = NULL;
	char* libAbsPath = NULL;

	do
	{
		FileSubsystem_Path gameJsonPath;
		wzl_sprintf(gameJsonPath, sizeof(gameJsonPath), "%s/game.json", dirPath);

		if ( !GameData_Load(gameJsonPath) )
		{
			break;
		}

		libAbsPath = ComputeGameLibraryAbsolutePath(dirPath);

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
