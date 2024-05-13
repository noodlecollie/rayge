#include "Subsystems/UISubsystem.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Subsystems/RendererSubsystem.h"
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

	return MeasureTextEx(RendererSubsystem_GetDefaultFont(), text, height, 0.0f).x;
}

void UISubsystem_Init(void)
{
	if ( g_Initialised )
	{
		return;
	}

	g_NKContext = InitNuklearEx(RendererSubsystem_GetDefaultFont(), RENDERSUBSYSTEM_DEFAULT_FONT_SIZE);
	RAYGE_ENSURE(g_NKContext, "Unable to create Nuklear context");

	g_CurrentMenu = NULL;
	g_Initialised = true;
}

void UISubsystem_ShutDown(void)
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

struct nk_context* UISubsystem_GetNuklearContext(void)
{
	return g_Initialised ? g_NKContext : NULL;
}

void UISubsystem_SetCurrentMenu(const RayGE_UIMenu* menu)
{
	if ( !g_Initialised || menu == g_CurrentMenu )
	{
		return;
	}

	UISubsystem_ClearCurrentMenu();

	g_CurrentMenu = menu;

	if ( g_CurrentMenu && g_CurrentMenu->Show )
	{
		g_CurrentMenu->Show(g_CurrentMenu->userData);
	}
}

void UISubsystem_ClearCurrentMenu(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	if ( g_CurrentMenu && g_CurrentMenu->Hide )
	{
		g_CurrentMenu->Hide(g_CurrentMenu->userData);
	}

	g_CurrentMenu = NULL;
}

void UISubsystem_Poll(void)
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
		g_CurrentMenu->Poll(g_CurrentMenu->userData);
	}

	DrawNuklear(g_NKContext);
}
