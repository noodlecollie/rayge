#include "UI/SceneDebugUI.h"
#include "Logging/Logging.h"
#include "Nuklear/Nuklear.h"
#include "raylib.h"

#define DEFAULT_MARGIN 20.0f

static struct nk_rect ComputeDefaultWindowSize(void)
{
	return (struct nk_rect) {
		DEFAULT_MARGIN,
		DEFAULT_MARGIN,
		(float)GetRenderWidth() / 2.0f,
		(float)GetRenderHeight() - (2.0f * DEFAULT_MARGIN),
	};
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
	(void)userData;

	bool shouldStayOpen = false;

	if ( nk_begin_titled(
			 context,
			 "debugui",
			 "Debug",
			 ComputeDefaultWindowSize(),
			 NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_CLOSABLE
		 ) )
	{
		shouldStayOpen = true;
	}

	nk_end(context);
	return shouldStayOpen;
}

const RayGE_UIMenu Menu_SceneDebugUI = {
	NULL,

	Show,
	Hide,
	Poll,
};
