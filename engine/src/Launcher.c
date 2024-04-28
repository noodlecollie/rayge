#include <stdio.h>
#include "RayGE/Engine.h"
#include "RayGE/Private/Launcher.h"
#include "RayGE/Private/CurrentEngineAPI.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Subsystems/FileSubsystem.h"
#include "GameLoader.h"
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
		"Default game directory %s was not found, and no game directory override was specified."
	);

	return NULL;
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

	int32_t returnCode = RAYGE_LAUNCHER_EXIT_OK;

	do
	{
		void* gameLib = LoadGameLibrary(params);

		if ( !gameLib )
		{
			LoggingSubsystem_PrintLine(
				RAYGE_LOG_ERROR,
				"Could not load game library."
			);

			break;
		}

		// TODO: Call init function on library
	}
	while ( false );

	LoggingSubsystem_ShutDown();

	return returnCode;
}
