#include <stdbool.h>
#include "Engine/Engine.h"
#include "Engine/EngineAPI.h"
#include "Logging/Logging.h"
#include "EngineSubsystems/EngineSubsystemManager.h"
#include "EngineSubsystems/UISubsystem.h"
#include "EngineSubsystems/InputSubsystem.h"
#include "EngineSubsystems/InputHookSubsystem.h"
#include "EngineSubsystems/SceneSubsystem.h"
#include "EngineSubsystems/RendererSubsystem.h"
#include "BehaviouralSubsystems/BSysManager.h"
#include "MemPool/MemPoolManager.h"
#include "Hooks/HookManager.h"
#include "Engine/EngineAPI.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Testing.h"
#include "Identity/Identity.h"
#include "Debugging.h"
#include "wzl_cutl/memory.h"

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

static bool g_Initialised = false;

static void* WzlMalloc(size_t size)
{
	return MEMPOOL_MALLOC(MEMPOOL_WZL_CUTL, size);
}

static void WzlFree(void* ptr)
{
	MEMPOOL_FREE(ptr);
}

static void* WzlCalloc(size_t num, size_t size)
{
	return MEMPOOL_CALLOC(MEMPOOL_WZL_CUTL, num, size);
}

static void* WzlRealloc(void* ptr, size_t size)
{
	return MEMPOOL_REALLOC(MEMPOOL_WZL_CUTL, ptr, size);
}

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
			Logging_PrintLine(
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
		Logging_PrintLine(RAYGE_LOG_FATAL, "One or more engine API functions were missing, aborting.");
	}
}

static void RunFrameInput(void)
{
	InputSubsystem_NewFrame();
	InputSubsystem_ProcessInput();
	InputHookSubsystem_ProcessInput();
	UISubsystem_ProcessInput();
	UISubsystem_Poll();
}

static bool RunFrame(void)
{
	const bool windowShouldClose = RendererSubsystem_IsWindowCloseRequested();

	BSysManager_Invoke(BSYS_STAGE_DESERIALISATION);
	RunFrameInput();
	BSysManager_Invoke(BSYS_STAGE_LOGIC);
	BSysManager_Invoke(BSYS_STAGE_SIMULATION);
	BSysManager_Invoke(BSYS_STAGE_RENDERING);
	BSysManager_Invoke(BSYS_STAGE_SERIALISATION);

	return windowShouldClose;
}

void Engine_StartUp(void)
{
	if ( g_Initialised )
	{
		return;
	}

	// Ensure the memory delegates are set up before we do anything else.
	wzl_set_memory_delegates((wzl_memory_delegates) {
		.malloc_func = WzlMalloc,
		.free_func = WzlFree,
		.calloc_func = WzlCalloc,
		.realloc_func = WzlRealloc,
	});

	Logging_Init();
	Logging_PrintLine(RAYGE_LOG_INFO, "%s", Identity_GetBuildDescription());

	VerifyAllEngineAPIFunctionPointersAreValid();

	MemPoolManager_Init();
	EngineSubsystemManager_InitAll();
	HookManager_RegisterAll();

	g_Initialised = true;

	Logging_PrintLine(RAYGE_LOG_INFO, "RayGE engine initialised.");

#if RAYGE_BUILD_TESTING()
	Logging_PrintLine(RAYGE_LOG_WARNING, "Engine is built with test endpoints exposed.");
#endif
}

void Engine_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	Logging_PrintLine(RAYGE_LOG_INFO, "RayGE engine shutting down.");

	HookManager_UnregisterAll();
	EngineSubsystemManager_ShutDownAll();
	MemPoolManager_ShutDown();

	Logging_ShutDown();

	g_Initialised = false;
}

void Engine_RunToCompletion(void)
{
	// TODO: These callbacks should probably be elsewhere
	INVOKE_CALLBACK(g_GameLibCallbacks.scene.SceneBegin);

	bool windowShouldClose = false;

	do
	{
		windowShouldClose = RunFrame();
	}
	while ( !windowShouldClose );

	INVOKE_CALLBACK(g_GameLibCallbacks.scene.SceneEnd);
}
