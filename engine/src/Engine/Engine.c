#include <stdbool.h>
#include "Engine/Engine.h"
#include "Engine/EngineAPI.h"
#include "Logging/Logging.h"
#include "Modules/ModuleManager.h"
#include "Modules/UIModule.h"
#include "Modules/InputModule.h"
#include "Modules/InputHookModule.h"
#include "Modules/SceneModule.h"
#include "Hooks/HookManager.h"
#include "Engine/EngineAPI.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Testing.h"
#include "Identity/Identity.h"

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
static void VisualiseEntities(void)
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

	RayGE_Renderer* renderer = RendererModule_GetRenderer();
	Renderer_SetBackgroundColour(renderer, BLACK);

	Camera3D camera = {0};

	camera.position = (Vector3) {20.0f, -20.0f, 10.0f};
	camera.target = (Vector3) {0.0f, 0.0f, 0.0f};
	camera.up = (Vector3) {0.0f, 0.0f, 1.0f};
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	Renderer_Set3DCamera(renderer, camera);

	Renderer_BeginFrame(renderer);
	Renderer_DrawAllActiveEntities(renderer);
	Renderer_EndFrame(renderer);
}

static void RunFrameInput(void)
{
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
	BeginDrawing();

	VisualiseEntities();
	UIModule_Draw();

	EndDrawing();
}

static bool RunFrame(void)
{
	bool windowShouldClose = RendererModule_WindowCloseRequested();

	RunFrameInput();
	RunFrameRender();

	return windowShouldClose;
}

void Engine_StartUp(void)
{
	if ( g_Initialised )
	{
		return;
	}

	// Do this first, as a sanity check:
	Logging_Init();
	Logging_PrintLine(RAYGE_LOG_INFO, "%s", Identity_GetBuildDescription());

	VerifyAllEngineAPIFunctionPointersAreValid();

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

	Logging_PrintLine(RAYGE_LOG_INFO, "RayGE engine shutting down.");

	HookManager_UnregisterAll();
	ModuleManager_ShutDownAll();

	Logging_ShutDown();

	g_Initialised = false;
}

void Engine_RunToCompletion(void)
{
	INVOKE_CALLBACK(g_GameLibCallbacks.scene.SceneBegin);

	bool windowShouldClose = false;

	do
	{
		windowShouldClose = RunFrame();
	}
	while ( !windowShouldClose );

	INVOKE_CALLBACK(g_GameLibCallbacks.scene.SceneEnd);
}
