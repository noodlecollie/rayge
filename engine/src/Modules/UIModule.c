#include "Modules/UIModule.h"
#include "Modules/MemPoolModule.h"
#include "Modules/RendererModule.h"
#include "Debugging.h"
#include "raylib.h"
#include "Nuklear/Nuklear.h"

static struct nk_context* g_NKContext = NULL;
static struct nk_user_font g_NKFont;
static const RayGE_UIMenu* g_CurrentMenu = NULL;
static bool g_Initialised = false;

static void* LocalAllocate(nk_handle handle, void* ptr, nk_size size)
{
	(void)handle;
	(void)ptr;

	return MEMPOOL_MALLOC(MEMPOOL_UI, size);
}

static void LocalFree(nk_handle handle, void* ptr)
{
	(void)handle;

	MEMPOOL_FREE(ptr);
}

static float ComputeTextWidthForDefaultFont(nk_handle handle, float height, const char* text, int textLength)
{
	(void)handle;
	(void)textLength;

	return MeasureTextEx(RendererModule_GetDefaultUIFont(), text, height, 0.0f).x;
}

void UIModule_Init(void)
{
	if ( g_Initialised )
	{
		return;
	}

	g_NKContext = InitNuklearEx(RendererModule_GetDefaultUIFont(), RENDERERMODULE_DEFAULT_FONT_SIZE);
	RAYGE_ENSURE(g_NKContext, "Unable to create Nuklear context");

	g_CurrentMenu = NULL;
	g_Initialised = true;
}

void UIModule_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	g_CurrentMenu = NULL;

	UnloadNuklear(g_NKContext);
	g_NKContext = NULL;

	g_Initialised = false;
}

const RayGE_UIMenu* UIModule_GetCurrentMenu(void)
{
	if ( !g_Initialised )
	{
		return NULL;
	}

	return g_CurrentMenu;
}

void UIModule_SetCurrentMenu(const RayGE_UIMenu* menu)
{
	if ( !g_Initialised || menu == g_CurrentMenu )
	{
		return;
	}

	UIModule_ClearCurrentMenu();

	g_CurrentMenu = menu;

	if ( g_CurrentMenu && g_CurrentMenu->Show )
	{
		g_CurrentMenu->Show(g_NKContext, g_CurrentMenu->userData);
	}
}

void UIModule_ClearCurrentMenu(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	if ( g_CurrentMenu && g_CurrentMenu->Hide )
	{
		g_CurrentMenu->Hide(g_NKContext, g_CurrentMenu->userData);
	}

	g_CurrentMenu = NULL;
}

bool UIModule_HasCurrentMenu(void)
{
	return g_Initialised && g_CurrentMenu;
}

void UIModule_PollCurrentMenu(void)
{
	if ( !g_Initialised || !g_CurrentMenu || !g_CurrentMenu->Poll )
	{
		return;
	}

	const bool shouldStayOpen = g_CurrentMenu->Poll(g_NKContext, g_CurrentMenu->userData);

	if ( !shouldStayOpen )
	{
		UIModule_ClearCurrentMenu();
	}
}

void UIModule_ProcessInput(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	UpdateNuklear(g_NKContext);
}

void UIModule_Draw(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	DrawNuklear(g_NKContext);
}

void UIModule_Poll(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	UpdateNuklear(g_NKContext);

	if ( !g_CurrentMenu )
	{
		return;
	}

	if ( g_CurrentMenu->Poll )
	{
		g_CurrentMenu->Poll(g_NKContext, g_CurrentMenu->userData);
	}

	DrawNuklear(g_NKContext);
}
