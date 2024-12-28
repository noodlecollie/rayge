#include <stddef.h>
#include <stdarg.h>
#include <float.h>
#include "UI/SceneDebugUI.h"
#include "Logging/Logging.h"
#include "Nuklear/Nuklear.h"
#include "EngineSubsystems/RendererSubsystem.h"
#include "EngineSubsystems/SceneSubsystem.h"
#include "UI/UIHelpers.h"
#include "wzl_cutl/string.h"
#include "raylib.h"

typedef struct WindowState
{
	float doubleCameraFOV;
	Vector3 cameraPos;
} WindowState;

static WindowState g_WindowState;

static void VAFormRow(struct nk_context* context, const char* label, const char* valueFormat, ...)
{
	nk_label(context, label, NK_TEXT_ALIGN_LEFT);

	char buffer[32];

	va_list args;
	va_start(args, valueFormat);
	wzl_vsprintf(buffer, sizeof(buffer), valueFormat, args);
	va_end(args);

	nk_label(context, buffer, NK_TEXT_ALIGN_LEFT);
}

static void BeginDynamicGroup(struct nk_context* context, int numInternalRows)
{
	nk_layout_row_dynamic(context, UI_TITLED_GROUPBOX_MIN_HEIGHT + ((float)numInternalRows * UI_DEFAULT_ROW_HEIGHT), 1);
}

static uint64_t CheckDebugFlag(struct nk_context* context, const char* label, uint64_t existingFlags, uint64_t flag)
{
	if ( nk_check_label(context, label, (existingFlags & flag) != 0) )
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

static void DrawFrameStatsGroup(struct nk_context* context)
{
	BeginDynamicGroup(context, 2);

	if ( nk_group_begin_titled(context, "frame_state", "Frame Stats", UI_DEFAULT_GROUP_FLAGS) )
	{
		const RayGE_Scene* scene = SceneSubsystem_GetScene();

		const float ratios[] = {0.70f, 0.30f};
		nk_layout_row(context, NK_DYNAMIC, UI_DEFAULT_ROW_HEIGHT, 2, ratios);

		VAFormRow(context, "FPS:", "%d", GetFPS());
		VAFormRow(context, "Active entities:", "%zu", Scene_GetActiveEntities(scene));

		nk_group_end(context);
	}
}

static void DrawCameraOverrideGroup(struct nk_context* context, WindowState* state)
{
	nk_layout_row_dynamic(context, 150.0f, 1);

	if ( nk_group_begin_titled(context, "camera_override", "Camera Override", UI_DEFAULT_GROUP_FLAGS) )
	{
		nk_layout_row_dynamic(context, UI_DEFAULT_ROW_HEIGHT, 1);

		nk_property_float(context, "FOV", 5.0f, &state->doubleCameraFOV, 160.0f, 0.5f, 0.5f);
		nk_property_float(context, "X", -FLT_MAX, &state->cameraPos.x, FLT_MAX, 1.0f, 1.0f);
		nk_property_float(context, "Y", -FLT_MAX, &state->cameraPos.y, FLT_MAX, 1.0f, 1.0f);
		nk_property_float(context, "Z", -FLT_MAX, &state->cameraPos.z, FLT_MAX, 1.0f, 1.0f);

		nk_group_end(context);
	}
}

static void DrawRenderDebugFlagGroup(struct nk_context* context)
{
	BeginDynamicGroup(context, 2);

	if ( nk_group_begin_titled(context, "render_debug_flags", "Render Debug Flags", UI_DEFAULT_GROUP_FLAGS) )
	{
		RayGE_Renderer* renderer = RendererSubsystem_GetRenderer();
		uint64_t debugFlags = Renderer_GetDebugFlags(renderer);

		nk_layout_row_dynamic(context, UI_DEFAULT_ROW_HEIGHT, 1);

		debugFlags = CheckDebugFlag(context, "Draw Locations", debugFlags, RENDERER_DBG_DRAW_LOCATIONS);
		debugFlags = CheckDebugFlag(context, "Override Camera", debugFlags, RENDERER_DBG_OVERRIDE_CAMERA);

		Renderer_SetDebugFlags(renderer, debugFlags);

		nk_group_end(context);
	}
}

static void Show(struct nk_context* context, void* userData)
{
	(void)context;
	(void)userData;

	Logging_PrintLine(RAYGE_LOG_TRACE, "Showing scene debug UI");
}

static void Hide(struct nk_context* context, void* userData)
{
	(void)context;
	(void)userData;

	Logging_PrintLine(RAYGE_LOG_TRACE, "Hiding scene debug UI");
}

static bool Poll(struct nk_context* context, void* userData)
{
	WindowState* windowState = (WindowState*)userData;
	bool shouldStayOpen = false;

	if ( nk_begin_titled(
			 context,
			 "debugui",
			 "Debug",
			 (struct nk_rect) {0.0f, 0.0f, 250.0f, (float)GetRenderHeight()},
			 UI_DEFAULT_PANE_FLAGS | NK_WINDOW_CLOSABLE
		 ) )
	{
		shouldStayOpen = true;
		UpdateWindowState(windowState);

		DrawFrameStatsGroup(context);
		DrawCameraOverrideGroup(context, windowState);
		DrawRenderDebugFlagGroup(context);

		ApplyWindowState(windowState);
	}

	nk_end(context);
	return shouldStayOpen;
}

const RayGE_UIMenu Menu_SceneDebugUI = {
	&g_WindowState,

	Show,
	Hide,
	Poll,
};
