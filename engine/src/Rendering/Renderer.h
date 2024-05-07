#pragma once

#include "RayGE/SceneTypes.h"
#include "Scene/Entity.h"
#include "raylib.h"

typedef enum Renderer_DebugFlag
{
	RENDERER_DBG_DRAW_LOCATIONS = (1 << 0),
} Renderer_DebugFlag;

void Renderer_AddDebugFlags(uint64_t flags);
void Renderer_RemoveDebugFlags(uint64_t flags);
void Renderer_ClearDebugFlags(void);

void Renderer_DrawTextDev(int posX, int posY, Color color, const char* text);
void Renderer_FormatTextDev(int posX, int posY, Color color, const char* format, ...);

void Renderer_DrawEntity(RayGE_Entity* entity);
