#include <stdio.h>
#include "RayGE/APIs/Engine.h"
#include "RayGE/Private/Launcher.h"
#include "RayGE/Private/CurrentEngineAPI.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Subsystems/FileSubsystem.h"
#include "Launcher/LaunchParams.h"
#include "Game/GameLoader.h"
#include "Game/GameWindow.h"
#include "EngineAPI.h"

#define NUM_ENGINE_API_FUNCTIONS (sizeof(RayGE_Engine_API_Current) / sizeof(void*))
#define DEFAULT_GAME_DIR "games/defaultgame"

typedef struct EngineAPIOpaqueFunctionTable
{
	void* funcPtrs[NUM_ENGINE_API_FUNCTIONS];
} EngineAPIOpaqueFunctionTable;

typedef union EngineAPIVerifyWrapper
{
	const RayGE_Engine_API_Current* apiPtr;
	const EngineAPIOpaqueFunctionTable* funcTablePtr;
} EngineAPIVerifyWrapper;

static void VerifyAllEngineAPIFunctionPointersAreValid(void)
{
	static_assert(
		sizeof(RayGE_Engine_API_Current) == sizeof(EngineAPIOpaqueFunctionTable),
		"Expected verifier struct to be same size as engine API struct"
	);

	EngineAPIVerifyWrapper wrapper;
	wrapper.apiPtr = &g_EngineAPI;

	bool functionWasInvalid = false;

	for ( size_t index = 0; index < NUM_ENGINE_API_FUNCTIONS; ++index )
	{
		if ( !wrapper.funcTablePtr->funcPtrs[index] )
		{
			LoggingSubsystem_PrintLine(
				RAYGE_LOG_ERROR,
				"Engine API function was null! (Table index: %zu, table size: %zu)",
				index,
				NUM_ENGINE_API_FUNCTIONS
			);

			functionWasInvalid = true;
		}
	}

	if ( functionWasInvalid )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_FATAL, "One or more engine API functions were missing, aborting.");
	}
}

static void SanityCheckEngineBeforeRunningInitProcedure(void)
{
	VerifyAllEngineAPIFunctionPointersAreValid();
}

static void* LoadGameLibrary(const RayGE_LaunchParams* params)
{
	// TODO: Support loading from a specific directory if passed in params.
	(void)params;

	if ( FileSubsystem_DirectoryExists(DEFAULT_GAME_DIR) )
	{
		return GameLoader_LoadLibraryFromDirectory(DEFAULT_GAME_DIR);
	}

	LoggingSubsystem_PrintLine(
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
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Could not load game library.");
		return RAYGE_LAUNCHER_EXIT_FAIL_GAME_LOAD;
	}

	INVOKE_CALLBACK(g_GameLibCallbacks.game.StartUp);

	GameWindow_CreateStatic();

	// TODO: Actually run game here

	GameWindow_DestroyStatic();

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

	// This must go first, so that we can log if required on init.
	LoggingSubsystem_Init();

	SanityCheckEngineBeforeRunningInitProcedure();
	LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "RayGE engine initialised.");

	int32_t returnCode = RAYGE_LAUNCHER_EXIT_UNKNOWN_ERROR;

	if ( !LaunchParams_Parse(params) )
	{
		// We don't need to go any further than parsing the params.
		returnCode = RAYGE_LAUNCHER_EXIT_LOAD_ABORTED;
	}
	else
	{
		returnCode = LoadAndRunGame(params);
	}

	LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "RayGE engine shutting down.");
	LoggingSubsystem_ShutDown();

	return returnCode;
}
