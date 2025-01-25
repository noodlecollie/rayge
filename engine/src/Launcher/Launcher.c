#include "Logging/Logging.h"
#include "EngineSubsystems/FilesystemSubsystem.h"
#include "Launcher/LaunchParams.h"
#include "Game/GameLoader.h"
#include "Engine/EngineAPI.h"
#include "Engine/Engine.h"
#include "Testing/Testing.h"

#define DEFAULT_GAME_DIR "games/defaultgame"

static void* LoadGameLibrary(const RayGE_LaunchParams* params)
{
	// TODO: Support loading from a specific directory if passed in params.
	(void)params;

	if ( FilesystemSubsystem_DirectoryExists(DEFAULT_GAME_DIR) )
	{
		return GameLoader_LoadLibraryFromDirectory(DEFAULT_GAME_DIR);
	}

	Logging_PrintLine(
		RAYGE_LOG_ERROR,
		"Default game directory " DEFAULT_GAME_DIR " was not found, and no game directory override was specified."
	);

	return NULL;
}

static int32_t LoadAndRunGame(const RayGE_LaunchParams* params)
{
	void* gameLib = LoadGameLibrary(params);

	if ( !gameLib )
	{
		Logging_PrintLine(RAYGE_LOG_ERROR, "Could not load game library.");
		return RAYGE_LAUNCHER_EXIT_FAIL_GAME_LOAD;
	}

	INVOKE_CALLBACK(g_GameLibCallbacks.game.StartUp);

	Engine_RunToCompletion();

	INVOKE_CALLBACK(g_GameLibCallbacks.game.ShutDown);

	GameLoader_UnloadLibrary(gameLib);
	return RAYGE_LAUNCHER_EXIT_OK;
}

RAYGE_ENGINE_PUBLIC(int32_t) RayGE_Launcher_Run(const RayGE_LaunchParams* params)
{
	if ( !RAYGE_INTERFACE_VERIFY(params, RAYGE_LAUNCHPARAMS_VERSION) )
	{
		return -RAYGE_LAUNCHPARAMS_VERSION;
	}

	if ( !LaunchParams_Parse(params) )
	{
		// We don't need to go any further than parsing the params.
		return RAYGE_LAUNCHER_EXIT_LOAD_ABORTED;
	}

#if RAYGE_BUILD_TESTING()
	if ( LaunchParams_GetLaunchState()->runTestsAndExit )
	{
		return Engine_RunTestsOnly();
	}
#endif()

	Engine_StartUp();
	int32_t returnCode = LoadAndRunGame(params);
	Engine_ShutDown();

	return returnCode;
}
