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

const RayGE_UIMenu* UISubsystem_GetCurrentMenu(void);
void UISubsystem_SetCurrentMenu(const RayGE_UIMenu* menu);
void UISubsystem_ClearCurrentMenu(void);
bool UISubsystem_HasCurrentMenu(void);
void UISubsystem_PollCurrentMenu(void);

void UISubsystem_ProcessInput(void);
void UISubsystem_Draw(void);
