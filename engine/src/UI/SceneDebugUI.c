#include <stddef.h>
#include "UI/SceneDebugUI.h"
#include "Logging/Logging.h"
#include "Nuklear/Nuklear.h"
#include "Modules/RendererModule.h"
#include "UI/UIHelpers.h"
#include "raylib.h"

#define DEFAULT_DIMS \
	(struct nk_rect) \
	{ \
		20.0f, 20.0f, 250.0f, 150.0f \
	}

typedef struct WindowState
{
	struct nk_rect lastRect;
} WindowState;

static WindowState g_WindowState;

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

static void DrawRenderDebugFlagGroup(struct nk_context* context)
{
	if ( nk_group_begin_titled(context, "render_debug_flags", "Render Debug Flags", UI_DEFAULT_GROUP_FLAGS) )
	{
		RayGE_Renderer* renderer = RendererModule_GetRenderer();
		uint64_t debugFlags = Renderer_GetDebugFlags(renderer);

		nk_layout_row_dynamic(context, 20.0f, 1);

		debugFlags = CheckDebugFlag(context, "Draw Locations", debugFlags, RENDERER_DBG_DRAW_LOCATIONS);

		Renderer_SetDebugFlags(renderer, debugFlags);

		nk_group_end(context);
	}
}

static bool Poll(struct nk_context* context, void* userData)
{
	WindowState* windowState = (WindowState*)userData;
	bool shouldStayOpen = false;

	if ( nk_begin_titled(
			 context,
			 "debugui",
			 "Debug",
			 UI_CalcWindowDims(windowState->lastRect, DEFAULT_DIMS),
			 UI_DEFAULT_WINDOW_FLAGS
		 ) )
	{
		shouldStayOpen = true;
		windowState->lastRect = nk_window_get_bounds(context);

		nk_layout_row_dynamic(context, 80.0f, 1);
		DrawRenderDebugFlagGroup(context);
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
