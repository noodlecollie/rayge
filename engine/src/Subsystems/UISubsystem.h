#pragma once

#include "Nuklear/Nuklear.h"

typedef struct RayGE_UIMenu
{
	void* userData;

	void (*Show)(struct nk_context* context, void* userData);
	void (*Hide)(struct nk_context* context, void* userData);
	void (*Poll)(struct nk_context* context, void* userData);
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
