#include "UI/ImGuiDemo.h"
#include "cimgui.h"

static bool g_WindowActive = false;

static void Show(struct nk_context* context, void* userData)
{
	(void)context;

	bool* windowActive = (bool*)userData;
	*windowActive = true;
}

static void Hide(struct nk_context* context, void* userData)
{
	(void)context;

	bool* windowActive = (bool*)userData;
	*windowActive = false;
}

static bool Poll(struct nk_context* context, void* userData)
{
	(void)context;

	bool* windowActive = (bool*)userData;

	if ( *windowActive )
	{
		igShowDemoWindow(windowActive);
	}

	return *windowActive;
}

const RayGE_UIMenu Menu_ImGuiDemo = {
	&g_WindowActive,

	Show,
	Hide,
	Poll,
};
