#include "EngineSubsystems/UISubsystem.h"
#include "MemPool/MemPoolManager.h"
#include "EngineSubsystems/RendererSubsystem.h"
#include "Debugging.h"
#include "cimgui.h"
#include "raylib.h"
#include "Integrations/ImGuiBackend.h"

typedef struct Data
{
	struct nk_context* nkContext;
	const RayGE_UIMenu* currentMenu;
	const RayGE_UIMenu* nextMenu;
	bool nextMenuWaiting;
	bool inPoll;
} Data;

static Data g_Data;
static bool g_Initialised = false;

static void SwitchMenu(Data* data, const RayGE_UIMenu* newMenu)
{
	if ( data->currentMenu == newMenu )
	{
		return;
	}

	if ( data->currentMenu && data->currentMenu->Hide )
	{
		data->currentMenu->Hide(data->currentMenu->userData);
	}

	data->currentMenu = newMenu;

	if ( data->currentMenu && data->currentMenu->Show )
	{
		data->currentMenu->Show(data->currentMenu->userData);
	}
}

void UISubsystem_Init(void)
{
	if ( g_Initialised )
	{
		return;
	}

	memset(&g_Data, 0, sizeof(g_Data));

	ImGui_ImplRaylib_Init();
	ImGui_ImplRaylib_BuildFontAtlas();

	g_Initialised = true;
}

void UISubsystem_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	ImGui_ImplRaylib_Shutdown();
	memset(&g_Data, 0, sizeof(g_Data));

	g_Initialised = false;
}

const RayGE_UIMenu* UISubsystem_GetCurrentMenu(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return NULL;
	}

	return g_Data.currentMenu;
}

void UISubsystem_SetCurrentMenu(const RayGE_UIMenu* menu)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised || menu == g_Data.currentMenu )
	{
		return;
	}

	if ( g_Data.inPoll )
	{
		g_Data.nextMenu = menu;
		g_Data.nextMenuWaiting = true;
		return;
	}

	SwitchMenu(&g_Data, menu);
}

void UISubsystem_ClearCurrentMenu(void)
{
	UISubsystem_SetCurrentMenu(NULL);
}

bool UISubsystem_HasCurrentMenu(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	return g_Initialised && g_Data.currentMenu;
}

void UISubsystem_PollCurrentMenu(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	// Must always be called, so that ImGui is in a known state for the draw call later.
	ImGui_ImplRaylib_NewFrame();

	if ( !g_Data.currentMenu || !g_Data.currentMenu->Poll )
	{
		return;
	}

	g_Data.inPoll = true;
	const bool shouldStayOpen = g_Data.currentMenu->Poll(g_Data.currentMenu->userData);
	g_Data.inPoll = false;

	if ( !shouldStayOpen )
	{
		SwitchMenu(&g_Data, NULL);
	}

	// Check if any new menu was requested during the poll.
	if ( g_Data.nextMenuWaiting )
	{
		SwitchMenu(&g_Data, g_Data.nextMenu);
		g_Data.nextMenu = NULL;
		g_Data.nextMenuWaiting = false;
	}
}

void UISubsystem_ProcessInput(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	ImGui_ImplRaylib_ProcessEvents();
}

void UISubsystem_Draw(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	RayGE_Renderer* renderer = RendererSubsystem_GetRenderer();

	RAYGE_ASSERT_VALID(renderer && Renderer_IsInFrame(renderer));

	if ( !renderer || !Renderer_IsInFrame(renderer) )
	{
		return;
	}

	Renderer_SetDrawingModeDirect(renderer);
	ImGui_ImplRaylib_Render();
}
