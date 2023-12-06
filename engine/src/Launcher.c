#include <stdio.h>
#include "RayGE/Engine.h"
#include "RayGE/Private/Launcher.h"
#include "Platform/FileSystem.h"
#include "Platform/FileSystemNative.h"
#include "Subsystems/LoggingSubsystem.h"
#include "EngineAPI.h"
#include "cJSON/cJSON.h"

#define NUM_ENGINE_API_FUNCTIONS (sizeof(RayGE_Engine_API) / sizeof(void*))
#define DEFAULT_GAME_DIR "games/defaultgame"

typedef struct EngineAPIOpaqueFunctionTable
{
	void* funcPtrs[NUM_ENGINE_API_FUNCTIONS];
} EngineAPIOpaqueFunctionTable;

typedef union EngineAPIVerifyWrapper
{
	const RayGE_Engine_API* apiPtr;
	const EngineAPIOpaqueFunctionTable* funcTablePtr;
} EngineAPIVerifyWrapper;

static bool TryLoadGameFromDirectory(const char* directory)
{
	Platform_Path path;

	sprintf_s(path, sizeof(path), "%s/game.json", directory);

	if ( !Platform_FileExists(path) )
	{
		// TODO: Log
		return false;
	}

	// TODO: Parse JSON to get library name
	// TODO: Load library by name
	// TODO: Pass interface to library
	return false;
}

static void VerifyAllEngineAPIFunctionPointersAreValid(void)
{
	static_assert(
		sizeof(RayGE_Engine_API) == sizeof(EngineAPIOpaqueFunctionTable),
		"Expected verifier struct to be same size as engine API struct"
	);

	EngineAPIVerifyWrapper wrapper;
	wrapper.apiPtr = &g_EngineAPI;

	bool functionWasInvalid = false;

	for ( size_t index = 0; index < NUM_ENGINE_API_FUNCTIONS; ++index )
	{
		if ( !wrapper.funcTablePtr->funcPtrs[index] )
		{
			LoggingSubsystem_EmitMessage(
				RAYGE_LOG_ERROR,
				"Engine API function was null! (Table index: %zu, table size: %zu)\n",
				index,
				NUM_ENGINE_API_FUNCTIONS
			);

			functionWasInvalid = true;
		}
	}

	if ( functionWasInvalid )
	{
		LoggingSubsystem_EmitMessage(RAYGE_LOG_FATAL, "One or more engine API functions were missing, aborting.\n");
	}
}

static void SanityCheckEngineBeforeRunningInitProcedure(void)
{
	VerifyAllEngineAPIFunctionPointersAreValid();
}

RAYGE_ENGINE_PUBLIC(int32_t) RayGE_Launcher_Run(const RayGE_LaunchParams* params)
{
	if ( !RAYGE_INTERFACE_VERIFY(params, RAYGE_LAUNCHPARAMS_VERSION) || params->argc < 1 )
	{
		return -RAYGE_LAUNCHPARAMS_VERSION;
	}

	SanityCheckEngineBeforeRunningInitProcedure();

	Platform_SetExecutableFromArgV0(params->argv[0]);

	const char* gameDir = DEFAULT_GAME_DIR;

	if ( !Platform_DirectoryExists(gameDir) )
	{
		// TODO: Expect a command line argument to
		// specify the game to open
		return 1;
	}

	if ( !TryLoadGameFromDirectory(gameDir) )
	{
		// TODO: Better logging
		return 1;
	}

	// TODO
	return 1;
}
