#include "UI/ImGuiDemo.h"
#include "cimgui.h"

static bool g_WindowActive = false;

static void Show(void* userData)
{
	bool* windowActive = (bool*)userData;
	*windowActive = true;
}

static void Hide(void* userData)
{
	bool* windowActive = (bool*)userData;
	*windowActive = false;
}

static bool Poll(void* userData)
{
	bool* windowActive = (bool*)userData;

	if ( *windowActive )
	{
		igShowDemoWindow(windowActive);
	}

	return *windowActive;
}

const RayGE_UIMenu Menu_ImGuiDemo = {
	&g_WindowActive,

	NULL,  // Init
	NULL,  // ShutDown
	Show,
	Hide,
	Poll,
};
