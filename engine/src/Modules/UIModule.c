#include "Modules/UIModule.h"
#include "MemPool/MemPoolManager.h"
#include "Modules/RendererModule.h"
#include "Debugging.h"
#include "raylib.h"
#include "Nuklear/Nuklear.h"

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
		data->currentMenu->Hide(data->nkContext, data->currentMenu->userData);
	}

	data->currentMenu = newMenu;

	if ( data->currentMenu && data->currentMenu->Show )
	{
		data->currentMenu->Show(data->nkContext, data->currentMenu->userData);
	}
}

void UIModule_Init(void)
{
	if ( g_Initialised )
	{
		return;
	}

	memset(&g_Data, 0, sizeof(g_Data));

	g_Data.nkContext = InitNuklearEx(RendererModule_GetDefaultUIFont(), RENDERERMODULE_DEFAULT_FONT_SIZE);
	RAYGE_ENSURE(g_Data.nkContext, "Unable to create Nuklear context");

	g_Initialised = true;
}

void UIModule_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	UnloadNuklear(g_Data.nkContext);
	memset(&g_Data, 0, sizeof(g_Data));

	g_Initialised = false;
}

const RayGE_UIMenu* UIModule_GetCurrentMenu(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return NULL;
	}

	return g_Data.currentMenu;
}

void UIModule_SetCurrentMenu(const RayGE_UIMenu* menu)
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

void UIModule_ClearCurrentMenu(void)
{
	UIModule_SetCurrentMenu(NULL);
}

bool UIModule_HasCurrentMenu(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	return g_Initialised && g_Data.currentMenu;
}

void UIModule_PollCurrentMenu(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	if ( !g_Data.currentMenu || !g_Data.currentMenu->Poll )
	{
		return;
	}

	g_Data.inPoll = true;
	const bool shouldStayOpen = g_Data.currentMenu->Poll(g_Data.nkContext, g_Data.currentMenu->userData);
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

void UIModule_ProcessInput(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	UpdateNuklear(g_Data.nkContext);
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
	DrawNuklear(g_Data.nkContext);
}
