#include <stdbool.h>
#include "Engine/Engine.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Subsystems/SubsystemManager.h"
#include "Subsystems/UISubsystem.h"
#include "Subsystems/InputSubsystem.h"
#include "Subsystems/InputHookSubsystem.h"
#include "Hooks/HookManager.h"
#include "Engine/EngineAPI.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Testing.h"
#include "Identity/Identity.h"

// TODO: Remove this once we move the rendering elsewhere
#include "Subsystems/RendererSubsystem.h"
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

// TODO: Remove this once we move the rendering elsewhere
static void VisualiseEntities(void)
{
	RayGE_Entity* firstEnt = Scene_GetActiveEntity(0);

	if ( firstEnt )
	{
		RayGE_ComponentHeader* cmpHeader = Entity_GetFirstComponentOfType(firstEnt, RAYGE_COMPONENTTYPE_SPATIAL);

		if ( cmpHeader )
		{
			COMPONENTDATA_SPATIAL(cmpHeader)->angles.yaw += 3.0f;
		}
	}

	Renderer_AddDebugFlags(RENDERER_DBG_DRAW_LOCATIONS);
	ClearBackground(BLACK);

	Camera3D camera = {0};

	camera.position = (Vector3) {20.0f, -20.0f, 10.0f};
	camera.target = (Vector3) {0.0f, 0.0f, 0.0f};
	camera.up = (Vector3) {0.0f, 0.0f, 1.0f};
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	BeginMode3D(camera);

	const size_t maxEntities = Scene_GetMaxEntities();

	for ( size_t index = 0; index < maxEntities; ++index )
	{
		RayGE_Entity* entity = Scene_GetActiveEntity(index);

		if ( !entity )
		{
			continue;
		}

		Renderer_DrawEntity(entity);
	}

	EndMode3D();

	Renderer_FormatTextDev(20, 20, WHITE, "Scene has %zu active entities", Scene_GetActiveEntities());
	Renderer_FormatTextDev(20, 40, WHITE, "FPS: %d", GetFPS());
}

static void RunFrameInput(void)
{
	InputSubsystem_NewFrame();

	// Don't allow any in-game keys to be pressed if UI is active.
	if ( UISubsystem_HasCurrentMenu() )
	{
		InputSubsystem_ClearAllInputThisFrame();
	}

	InputSubsystem_ProcessInput();
	InputHookSubsystem_ProcessInput();
}

static void RunFrameUIInput(void)
{
	// TODO: Remove this once finished testing
	if ( IsKeyPressed(KEY_GRAVE) )
	{
		if ( !UISubsystem_HasCurrentMenu() )
		{
			UISubsystem_SetCurrentMenu(&Menu_TestUI);
		}
		else
		{
			UISubsystem_ClearCurrentMenu();
		}
	}

	if ( UISubsystem_HasCurrentMenu() )
	{
		UISubsystem_ProcessInput();
		UISubsystem_PollCurrentMenu();
	}
}

static void RunFrameRender(void)
{
	BeginDrawing();

	// Render scene
	VisualiseEntities();

	// Poll and render UI
	if ( UISubsystem_HasCurrentMenu() )
	{
		UISubsystem_Draw();
	}

	EndDrawing();
}

static bool RunFrame(void)
{
	bool windowShouldClose = RendererSubsystem_WindowCloseRequested();

	RunFrameInput();
	RunFrameUIInput();
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
	LoggingSubsystem_Init();
	VerifyAllEngineAPIFunctionPointersAreValid();

	SubsystemManager_InitAll();
	HookManager_RegisterAll();

	g_Initialised = true;

	LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "RayGE engine initialised.");
	LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "%s", Identity_GetBuildDescription());

#if RAYGE_BUILD_TESTING()
	LoggingSubsystem_PrintLine(RAYGE_LOG_WARNING, "Engine is built with test endpoints exposed.");
#endif
}

void Engine_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "RayGE engine shutting down.");

	HookManager_UnregisterAll();
	SubsystemManager_ShutDownAll();

	LoggingSubsystem_ShutDown();

	g_Initialised = false;
}

void Engine_RunToCompletion(void)
{
	// TODO: Make this value canonical somehow?
	Scene_CreateStatic(1024);

	bool windowShouldClose = false;

	do
	{
		windowShouldClose = RunFrame();
	}
	while ( !windowShouldClose );

	Scene_DestroyStatic();
}
