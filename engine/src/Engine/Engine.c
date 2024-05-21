#include <stdbool.h>
#include "Engine/Engine.h"
#include "Engine/EngineAPI.h"
#include "Logging/Logging.h"
#include "Modules/ModuleManager.h"
#include "Modules/UIModule.h"
#include "Modules/InputModule.h"
#include "Modules/InputHookModule.h"
#include "Modules/SceneModule.h"
#include "MemPool/MemPoolManager.h"
#include "Hooks/HookManager.h"
#include "Engine/EngineAPI.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Testing.h"
#include "Identity/Identity.h"
#include "Debugging.h"

// TODO: Remove this once we move the rendering elsewhere
#include "Modules/RendererModule.h"
#include "Rendering/Renderer.h"
#include "raylib.h"
#include "UI/TestUI.h"

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
static RayGE_EngineState g_State = ENGINE_STATE_IDLE;

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

// TODO: Remove this once we move the rendering elsewhere
static void VisualiseEntities(RayGE_Renderer* renderer)
{
	RayGE_Scene* scene = SceneModule_GetScene();
	RayGE_Entity* firstEnt = Scene_GetActiveEntity(scene, 0);

	if ( firstEnt )
	{
		RayGE_ComponentHeader* cmpHeader = Entity_GetFirstComponentOfType(firstEnt, RAYGE_COMPONENTTYPE_SPATIAL);

		if ( cmpHeader )
		{
			COMPONENTDATA_SPATIAL(cmpHeader)->angles.yaw += 3.0f;
		}
	}

	Renderer_SetDrawingMode3D(renderer, Renderer_GetDefaultCamera3D());
	Renderer_DrawAllActiveEntitiesInScene3D(renderer);
}

static void RunFrameInput(void)
{
	RAYGE_ENSURE_VALID(g_State == ENGINE_STATE_PROCESSING_INPUT);

	InputModule_NewFrame();
	InputModule_ProcessInput();
	InputHookModule_ProcessInput();

	if ( UIModule_HasCurrentMenu() )
	{
		UIModule_ProcessInput();
		UIModule_PollCurrentMenu();
	}
}

static void RunFrameRender(void)
{
	RAYGE_ENSURE_VALID(g_State == ENGINE_STATE_RENDERING);

	RayGE_Renderer* renderer = RendererModule_GetRenderer();

	// Frame setup
	Renderer_SetBackgroundColour(renderer, BLACK);

	// Begin rendering frame
	Renderer_BeginFrame(renderer);

	// TODO: Remove this once rendering is formalised more
	VisualiseEntities(renderer);

	UIModule_Draw();

	// End rendering frame
	Renderer_EndFrame(renderer);
}

static bool RunFrame(void)
{
	RAYGE_ENSURE_VALID(g_State == ENGINE_STATE_INTER_FRAME);

	g_State = ENGINE_STATE_PROCESSING_INPUT;

	bool windowShouldClose = RendererModule_WindowCloseRequested();
	RunFrameInput();

	g_State = ENGINE_STATE_RENDERING;
	RunFrameRender();

	g_State = ENGINE_STATE_INTER_FRAME;
	return windowShouldClose;
}

void Engine_StartUp(void)
{
	if ( g_Initialised )
	{
		return;
	}

	g_State = ENGINE_STATE_IDLE;

	// Do this first, as a sanity check:
	Logging_Init();
	Logging_PrintLine(RAYGE_LOG_INFO, "%s", Identity_GetBuildDescription());

	VerifyAllEngineAPIFunctionPointersAreValid();

	MemPoolManager_Init();
	ModuleManager_InitAll();
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

	g_State = ENGINE_STATE_IDLE;

	Logging_PrintLine(RAYGE_LOG_INFO, "RayGE engine shutting down.");

	HookManager_UnregisterAll();
	ModuleManager_ShutDownAll();
	MemPoolManager_ShutDown();

	Logging_ShutDown();

	g_Initialised = false;
}

void Engine_RunToCompletion(void)
{
	RAYGE_ENSURE_VALID(g_State == ENGINE_STATE_IDLE);

	g_State = ENGINE_STATE_INTER_FRAME;

	// TODO: These callbacks should probably be elsewhere
	INVOKE_CALLBACK(g_GameLibCallbacks.scene.SceneBegin);

	bool windowShouldClose = false;

	do
	{
		windowShouldClose = RunFrame();
	}
	while ( !windowShouldClose );

	RAYGE_ENSURE_VALID(g_State == ENGINE_STATE_INTER_FRAME);
	INVOKE_CALLBACK(g_GameLibCallbacks.scene.SceneEnd);

	g_State = ENGINE_STATE_IDLE;
}

RayGE_EngineState Engine_GetCurrentState(void)
{
	return g_Initialised ? g_State : ENGINE_STATE_IDLE;
}
