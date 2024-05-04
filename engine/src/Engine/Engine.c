#include <stdbool.h>
#include "Engine/Engine.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Subsystems/SubsystemManager.h"
#include "Engine/EngineAPI.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

// TODO: Remove this once we move the rendering elsewhere
#include "Subsystems/RendererSubsystem.h"
#include "raylib.h"

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

static bool RunFrame(void)
{
	bool windowShouldClose = RenderSubsystem_WindowCloseRequested();

	// TODO: Remove this once we move the rendering elsewhere
	BeginDrawing();
	ClearBackground(BLACK);

	Camera3D camera = {0};

	camera.position = (Vector3) {20.0f, -20.0f, 5.0f};
	camera.target = (Vector3) {0.0f, 0.0f, 0.0f};
	camera.up = (Vector3) {0.0f, 0.0f, 1.0f};
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	BeginMode3D(camera);

	size_t maxEntities = Scene_GetMaxEntities();

	for ( size_t index = 0; index < maxEntities; ++index )
	{
		RayGE_Entity* entity = Scene_GetActiveEntity(index);

		if ( !entity )
		{
			continue;
		}

		RayGE_ComponentImpl_Spatial* spatial =
			COMPONENTCAST_SPATIAL(Entity_GetFirstComponentOfType(entity, RAYGE_COMPONENTTYPE_SPATIAL), false);

		if ( !spatial )
		{
			continue;
		}

		Vector3 start = {0, 0, 0};
		Vector3 end = {0, 0, 0};

		start = Vector3Add(spatial->data.position, (Vector3) {-3, 0, 0});
		end = Vector3Add(spatial->data.position, (Vector3) {3, 0, 0});
		DrawLine3D(start, end, RED);
		DrawCircle3D(end, 0.5f, (Vector3) {1, 0, 0}, 0.0f, RED);

		start = Vector3Add(spatial->data.position, (Vector3) {0, -3, 0});
		end = Vector3Add(spatial->data.position, (Vector3) {0, 3, 0});
		DrawLine3D(start, end, GREEN);
		DrawCircle3D(end, 0.5f, (Vector3) {0, 1, 0}, 0.0f, GREEN);

		start = Vector3Add(spatial->data.position, (Vector3) {0, 0, -3});
		end = Vector3Add(spatial->data.position, (Vector3) {0, 0, 3});
		DrawLine3D(start, end, BLUE);
		DrawCircle3D(end, 0.5f, (Vector3) {0, 0, 1}, 0.0f, BLUE);
	}

	EndMode3D();

	RendererSubsystem_DrawTextDev("This is some text", 20, 20, WHITE);

	EndDrawing();

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
