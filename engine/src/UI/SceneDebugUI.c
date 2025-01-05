#include <stddef.h>
#include <stdarg.h>
#include <float.h>
#include "UI/SceneDebugUI.h"
#include "Logging/Logging.h"
#include "EngineSubsystems/RendererSubsystem.h"
#include "EngineSubsystems/SceneSubsystem.h"
#include "UI/UIHelpers.h"
#include "wzl_cutl/string.h"
#include "raylib.h"
#include "cimgui.h"

typedef struct WindowState
{
	bool showPane;
	float doubleCameraFOV;
	Vector3 cameraPos;
} WindowState;

static WindowState g_WindowState;

static uint64_t CheckDebugFlag(const char* label, uint64_t existingFlags, uint64_t flag)
{
	bool flagState = (existingFlags & flag) != 0;
	igCheckbox(label, &flagState);

	if ( flagState )
	{
		return existingFlags | flag;
	}
	else
	{
		return existingFlags & ~flag;
	}
}

static void UpdateWindowState(WindowState* state)
{
	RayGE_Renderer* renderer = RendererSubsystem_GetRenderer();
	Camera3D debugCamera = Renderer_GetDebugCamera3D(renderer);

	state->doubleCameraFOV = debugCamera.fovy * 2.0f;
	state->cameraPos = debugCamera.position;
}

static void ApplyWindowState(WindowState* state)
{
	RayGE_Renderer* renderer = RendererSubsystem_GetRenderer();
	Camera3D debugCamera = Renderer_GetDebugCamera3D(renderer);

	debugCamera.fovy = state->doubleCameraFOV / 2.0f;
	debugCamera.position = state->cameraPos;

	Renderer_SetDebugCamera3D(renderer, debugCamera);
}

static void DrawFrameStatsGroup(void)
{
	const RayGE_Scene* scene = SceneSubsystem_GetScene();

	igSeparatorText("Frame Stats");
	igText("FPS: %d", GetFPS());
	igText("Active entities: %zu", Scene_GetActiveEntities(scene));
}

static void DrawCameraOverrideGroup(WindowState* state)
{
	igSeparatorText("Camera Override");
	igDragFloat("FOV", &state->doubleCameraFOV, 0.25f, 5.0f, 160.0f, "%.0f", 0);
	igDragFloat3("Position", (float*)&state->cameraPos, 1.0f, -FLT_MAX, FLT_MAX, "%.0f", 0);
}

static void DrawRenderDebugFlagGroup()
{
	RayGE_Renderer* renderer = RendererSubsystem_GetRenderer();
	uint64_t debugFlags = Renderer_GetDebugFlags(renderer);

	igSeparatorText("Render Debug Flags");

	debugFlags = CheckDebugFlag("Draw Locations", debugFlags, RENDERER_DBG_DRAW_LOCATIONS);
	debugFlags = CheckDebugFlag("Override Camera", debugFlags, RENDERER_DBG_OVERRIDE_CAMERA);

	Renderer_SetDebugFlags(renderer, debugFlags);
}

static void Show(struct nk_context* context, void* userData)
{
	(void)context;

	((WindowState*)userData)->showPane = true;

	Logging_PrintLine(RAYGE_LOG_TRACE, "Showing scene debug UI");
}

static void Hide(struct nk_context* context, void* userData)
{
	(void)context;

	((WindowState*)userData)->showPane = false;

	Logging_PrintLine(RAYGE_LOG_TRACE, "Hiding scene debug UI");
}

static bool Poll(struct nk_context* context, void* userData)
{
	(void)context;

	WindowState* windowState = (WindowState*)userData;

	if ( !windowState->showPane )
	{
		return false;
	}

	igSetNextWindowPos((ImVec2) {0.0f, 0.0f}, 0, (ImVec2) {0.0f, 0.0f});
	igSetNextWindowSize((ImVec2) {250.0f, (float)GetRenderHeight()}, 0);

	const ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse;

	if ( igBegin("Debug UI", NULL, winFlags) )
	{
		UpdateWindowState(windowState);

		DrawFrameStatsGroup();
		igSpacing();
		DrawCameraOverrideGroup(windowState);
		igSpacing();
		DrawRenderDebugFlagGroup();

		ApplyWindowState(windowState);
		igEnd();
	}

	return true;
}

const RayGE_UIMenu Menu_SceneDebugUI = {
	&g_WindowState,

	Show,
	Hide,
	Poll,
};
