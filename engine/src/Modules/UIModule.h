#pragma once

#include <stdbool.h>
#include "Nuklear/Nuklear.h"

typedef struct RayGE_UIMenu
{
	void* userData;

	void (*Show)(struct nk_context* context, void* userData);
	void (*Hide)(struct nk_context* context, void* userData);

	// Return false if the menu should close.
	bool (*Poll)(struct nk_context* context, void* userData);
} RayGE_UIMenu;

void UIModule_Init(void);
void UIModule_ShutDown(void);

const RayGE_UIMenu* UIModule_GetCurrentMenu(void);
void UIModule_SetCurrentMenu(const RayGE_UIMenu* menu);
void UIModule_ClearCurrentMenu(void);
bool UIModule_HasCurrentMenu(void);
void UIModule_PollCurrentMenu(void);

void UIModule_ProcessInput(void);
void UIModule_Draw(void);
