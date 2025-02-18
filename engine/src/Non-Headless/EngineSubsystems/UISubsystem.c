#include "Non-Headless/EngineSubsystems/UISubsystem.h"
#include "Non-Headless/EngineSubsystems/RendererSubsystem.h"
#include "EngineSubsystems/InputSubsystem.h"
#include "MemPool/MemPoolManager.h"
#include "Debugging.h"
#include "utlist.h"
#include "Non-Headless/Integrations/ImGuiBackend.h"

typedef struct MenuEntry
{
	struct MenuEntry* next;
	const RayGE_UIMenu* menu;
} MenuEntry;

typedef struct MenuLists
{
	MenuEntry* activeMenus;
	MenuEntry* menusToShow;
	MenuEntry* menusToHide;
} MenuLists;

typedef struct Data
{
	struct nk_context* nkContext;
	MenuLists menus;
	bool lockActiveMenus;
} Data;

static Data g_Data;
static bool g_Initialised = false;

static bool IsMenuInList(MenuEntry* head, const RayGE_UIMenu* menu)
{
	MenuEntry* entry = NULL;

	LL_FOREACH(head, entry)
	{
		if ( entry->menu == menu )
		{
			return true;
		}
	}

	return false;
}

static void AddMenuToList(MenuEntry** head, const RayGE_UIMenu* menu)
{
	if ( IsMenuInList(*head, menu) )
	{
		return;
	}

	MenuEntry* entry = MEMPOOL_MALLOC_STRUCT(MEMPOOL_UI, MenuEntry);
	entry->menu = menu;
	LL_APPEND(*head, entry);
}

static void DeleteMenuFromList(MenuEntry** head, const RayGE_UIMenu* menu)
{
	MenuEntry* element = NULL;
	MenuEntry* temp = NULL;

	LL_FOREACH_SAFE(*head, element, temp)
	{
		if ( element->menu == menu )
		{
			LL_DELETE(*head, element);
			MEMPOOL_FREE(element);
		}
	}
}

static void ClearList(MenuEntry** head)
{
	MenuEntry* element = NULL;
	MenuEntry* temp = NULL;

	LL_FOREACH_SAFE(*head, element, temp)
	{
		LL_DELETE(*head, element);
		MEMPOOL_FREE(element);
	}
}

static void DeleteMatchingItemsAcrossBothLists(MenuEntry** head1, MenuEntry** head2)
{
	MenuEntry* entry = NULL;
	MenuEntry* temp = NULL;

	LL_FOREACH_SAFE(*head1, entry, temp)
	{
		if ( IsMenuInList(*head2, entry->menu) )
		{
			DeleteMenuFromList(head2, entry->menu);

			LL_DELETE(*head1, entry);
			MEMPOOL_FREE(entry);
		}
	}
}

static void SwitchMenus(MenuLists* lists)
{
	DeleteMatchingItemsAcrossBothLists(&lists->menusToShow, &lists->menusToHide);

	MenuEntry* entry = NULL;

	LL_FOREACH(lists->menusToHide, entry)
	{
		if ( IsMenuInList(lists->activeMenus, entry->menu) )
		{
			if ( entry->menu->Hide )
			{
				entry->menu->Hide(entry->menu->userData);
			}

			DeleteMenuFromList(&lists->activeMenus, entry->menu);
		}
	}

	LL_FOREACH(lists->menusToShow, entry)
	{
		if ( !IsMenuInList(lists->activeMenus, entry->menu) )
		{
			AddMenuToList(&lists->activeMenus, entry->menu);

			if ( entry->menu->Show )
			{
				entry->menu->Show(entry->menu->userData);
			}
		}
	}

	ClearList(&lists->menusToHide);
	ClearList(&lists->menusToShow);
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

	ClearList(&g_Data.menus.activeMenus);
	ClearList(&g_Data.menus.menusToShow);
	ClearList(&g_Data.menus.menusToHide);

	memset(&g_Data, 0, sizeof(g_Data));

	g_Initialised = false;
}

void UISubsystem_ShowMenu(const RayGE_UIMenu* menu)
{
	RAYGE_ASSERT_VALID(g_Initialised && menu && menu->Poll);

	if ( !g_Initialised )
	{
		return;
	}

	if ( !menu )
	{
		Logging_PrintLine(RAYGE_LOG_WARNING, "UISubsystem_ShowMenu called with null menu!");
		return;
	}

	if ( !menu->Poll )
	{
		Logging_PrintLine(RAYGE_LOG_WARNING, "Cannot show menu without a poll function assigned!");
		return;
	}

	if ( g_Data.lockActiveMenus )
	{
		DeleteMenuFromList(&g_Data.menus.menusToHide, menu);
		AddMenuToList(&g_Data.menus.menusToShow, menu);
		return;
	}

	if ( IsMenuInList(g_Data.menus.activeMenus, menu) )
	{
		return;
	}

	g_Data.lockActiveMenus = true;

	AddMenuToList(&g_Data.menus.activeMenus, menu);
	InputSubsystem_SetCurrentInputLayer(INPUT_LAYER_UI);

	if ( menu->Show )
	{
		menu->Show(menu->userData);
	}

	g_Data.lockActiveMenus = false;
}

void UISubsystem_HideMenu(const RayGE_UIMenu* menu)
{
	RAYGE_ASSERT_VALID(g_Initialised && menu);

	if ( !g_Initialised )
	{
		return;
	}

	if ( !menu )
	{
		Logging_PrintLine(RAYGE_LOG_WARNING, "UISubsystem_HideMenu called with null menu!");
		return;
	}

	if ( g_Data.lockActiveMenus )
	{
		DeleteMenuFromList(&g_Data.menus.menusToShow, menu);
		AddMenuToList(&g_Data.menus.menusToHide, menu);
		return;
	}

	if ( !IsMenuInList(g_Data.menus.activeMenus, menu) )
	{
		return;
	}

	g_Data.lockActiveMenus = true;

	if ( menu->Hide )
	{
		menu->Hide(menu->userData);
	}

	DeleteMenuFromList(&g_Data.menus.activeMenus, menu);

	g_Data.lockActiveMenus = false;
}

void UISubsystem_HideAllMenus(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	if ( g_Data.lockActiveMenus )
	{
		MenuEntry* entry = NULL;

		LL_FOREACH(g_Data.menus.activeMenus, entry)
		{
			DeleteMenuFromList(&g_Data.menus.menusToShow, entry->menu);
			AddMenuToList(&g_Data.menus.menusToHide, entry->menu);
		}

		return;
	}

	g_Data.lockActiveMenus = true;

	MenuEntry* entry = NULL;

	LL_FOREACH(g_Data.menus.activeMenus, entry)
	{
		if ( entry->menu->Hide )
		{
			entry->menu->Hide(entry->menu->userData);
		}
	}

	ClearList(&g_Data.menus.activeMenus);

	g_Data.lockActiveMenus = false;
}

bool UISubsystem_HasActiveMenus(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	return g_Initialised && g_Data.menus.activeMenus != NULL;
}

bool UISubsystem_IsMenuActive(const RayGE_UIMenu* menu)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	return g_Initialised && IsMenuInList(g_Data.menus.activeMenus, menu);
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

void UISubsystem_Poll(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	// Must always be called, so that ImGui is in a known state for the draw call later.
	ImGui_ImplRaylib_NewFrame();

	MenuLists* lists = &g_Data.menus;

	if ( !lists->activeMenus )
	{
		return;
	}

	g_Data.lockActiveMenus = true;
	MenuEntry* entry = NULL;

	LL_FOREACH(lists->activeMenus, entry)
	{
		const bool shouldStayOpen = entry->menu->Poll(entry->menu->userData);

		if ( !shouldStayOpen )
		{
			UISubsystem_HideMenu(entry->menu);
		}
	}

	// Move the current lists out of the state struct
	// so that if any menu calls functions that affect
	// these lists, they don't affect them while
	// we're manipulating them. Any show/hide requests
	// will end up in the existing lists, and we will
	// concatenate these with anything we compute.
	MenuLists menus = g_Data.menus;
	memset(&g_Data.menus, 0, sizeof(g_Data.menus));

	SwitchMenus(&menus);

	// This should not have been modified in the meantime
	RAYGE_ENSURE(!g_Data.menus.activeMenus, "Active menus list was unexpectedly modified!");

	g_Data.menus.activeMenus = menus.activeMenus;

	LL_FOREACH(menus.menusToShow, entry)
	{
		AddMenuToList(&g_Data.menus.menusToShow, entry->menu);
	}

	LL_FOREACH(menus.menusToHide, entry)
	{
		AddMenuToList(&g_Data.menus.menusToHide, entry->menu);
	}

	ClearList(&menus.menusToShow);
	ClearList(&menus.menusToHide);

	InputSubsystem_SetCurrentInputLayer(UISubsystem_HasActiveMenus() ? INPUT_LAYER_UI : INPUT_LAYER_GAME);

	g_Data.lockActiveMenus = false;
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
