#include <stdbool.h>
#include "Engine/Engine.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Engine/EngineAPI.h"
#include "Utils.h"

#define NUM_ENGINE_API_FUNCTIONS (sizeof(RayGE_Engine_API_Current) / sizeof(void*))

typedef struct EngineAPIOpaqueFunctionTable
{
	void* funcPtrs[NUM_ENGINE_API_FUNCTIONS];
} EngineAPIOpaqueFunctionTable;

typedef union EngineAPIVerifyWrapper
{
	const RayGE_Engine_API_Current* apiPtr;
	const EngineAPIOpaqueFunctionTable* funcTablePtr;
} EngineAPIVerifyWrapper;

typedef void (*SubsystemFuncPtr)(void);

// The following exclude the logging subsystem, since this is handled independently.
// Subsystems are initialised and shut down in the order of the arrays.

static const SubsystemFuncPtr g_SubsystemInitFuncs[] =
{
	MemPoolSubsystem_Init
};

static const SubsystemFuncPtr g_SubsystemShutdownFuncs[] =
{
	MemPoolSubsystem_ShutDown
};

static bool g_Initialised = false;

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

void Engine_StartUp(void)
{
	if ( g_Initialised )
	{
		return;
	}

	// Do this first, as a sanity check:
	LoggingSubsystem_Init();
	VerifyAllEngineAPIFunctionPointersAreValid();

	for ( size_t index = 0; index < RAYGE_ARRAY_SIZE(g_SubsystemInitFuncs); ++index )
	{
		(g_SubsystemInitFuncs[index])();
	}

	g_Initialised = true;
	LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "RayGE engine initialised.");
}

void Engine_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "RayGE engine shutting down.");

	for ( size_t index = 0; index < RAYGE_ARRAY_SIZE(g_SubsystemShutdownFuncs); ++index )
	{
		(g_SubsystemShutdownFuncs[index])();
	}

	LoggingSubsystem_ShutDown();

	g_Initialised = false;
}
