#include "Modules/UIModule.h"
#include "Modules/MemPoolModule.h"
#include "Modules/RendererModule.h"
#include "Debugging.h"
#include "raylib.h"
#include "Nuklear/Nuklear.h"

static struct nk_context* g_NKContext = NULL;
static const RayGE_UIMenu* g_CurrentMenu = NULL;
static bool g_Initialised = false;

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
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return NULL;
	}

	return g_CurrentMenu;
}

void UIModule_SetCurrentMenu(const RayGE_UIMenu* menu)
{
	RAYGE_ASSERT_VALID(g_Initialised);

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
	RAYGE_ASSERT_VALID(g_Initialised);

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
	RAYGE_ASSERT_VALID(g_Initialised);

	return g_Initialised && g_CurrentMenu;
}

void UIModule_PollCurrentMenu(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

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
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	UpdateNuklear(g_NKContext);
}

void UIModule_Draw(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	RayGE_Renderer* renderer = RendererModule_GetRenderer();

	RAYGE_ASSERT_VALID(renderer && Renderer_IsInFrame(renderer));

	if ( !renderer || !Renderer_IsInFrame(renderer) )
	{
		return;
	}

	Renderer_SetDrawingModeDirect(renderer);
	DrawNuklear(g_NKContext);
}

void UIModule_Poll(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

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
