#pragma once

#include <stdbool.h>

typedef struct RayGE_UIMenu
{
	void* userData;

	void (*Init)(void* userData);
	void (*ShutDown)(void* userData);

	void (*Show)(void* userData);
	void (*Hide)(void* userData);

	// Return false if the menu should close.
	bool (*Poll)(void* userData);
} RayGE_UIMenu;

void UISubsystem_Init(void);
void UISubsystem_ShutDown(void);

void UISubsystem_ShowMenu(const RayGE_UIMenu* menu);
void UISubsystem_HideMenu(const RayGE_UIMenu* menu);
void UISubsystem_HideAllMenus(void);
bool UISubsystem_HasActiveMenus(void);
bool UISubsystem_IsMenuActive(const RayGE_UIMenu* menu);

void UISubsystem_ProcessInput(void);
void UISubsystem_Poll(void);
void UISubsystem_Draw(void);
