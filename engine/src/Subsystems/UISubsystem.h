#pragma once

#include "Nuklear/Nuklear.h"

typedef struct RayGE_UIMenu
{
	void* userData;

	void (*Show)(void* userData);
	void (*Hide)(void* userData);
	void (*Poll)(void* userData);
} RayGE_UIMenu;

void UISubsystem_Init(void);
void UISubsystem_ShutDown(void);
struct nk_context* UISubsystem_GetNuklearContext(void);

void UISubsystem_SetCurrentMenu(const RayGE_UIMenu* menu);
void UISubsystem_ClearCurrentMenu(void);
void UISubsystem_Poll(void);
