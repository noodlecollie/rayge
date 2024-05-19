#pragma once

#include "RayGE/SceneTypes.h"
#include "Scene/Entity.h"
#include "raylib.h"

#define RENDERCOLOUR_NONE BLANK

typedef struct RayGE_Renderer RayGE_Renderer;

typedef enum Renderer_DebugFlag
{
	RENDERER_DBG_DRAW_LOCATIONS = (1 << 0),
} Renderer_DebugFlag;

RayGE_Renderer* Renderer_Create(void);
void Renderer_Destroy(RayGE_Renderer* renderer);

void Renderer_AddDebugFlags(RayGE_Renderer* renderer, uint64_t flags);
void Renderer_RemoveDebugFlags(RayGE_Renderer* renderer, uint64_t flags);
void Renderer_SetDebugFlags(RayGE_Renderer* renderer, uint64_t flags);
void Renderer_ClearDebugFlags(RayGE_Renderer* renderer);
uint64_t Renderer_GetDebugFlags(const RayGE_Renderer* renderer);

void Renderer_Set2DCamera(RayGE_Renderer* renderer, Camera2D camera);
void Renderer_Set3DCamera(RayGE_Renderer* renderer, Camera3D camera);
void Renderer_ClearCamera(RayGE_Renderer* renderer);

void Renderer_SetBackgroundColour(RayGE_Renderer* renderer, Color colour);
void Renderer_ClearBackgroundColour(RayGE_Renderer* renderer);

void Renderer_BeginFrame(RayGE_Renderer* renderer);
void Renderer_EndFrame(RayGE_Renderer* renderer);

void Renderer_DrawEntity(RayGE_Renderer* renderer, RayGE_Entity* entity);
void Renderer_DrawAllActiveEntities(RayGE_Renderer* renderer);

// These functions do not need to happen within a frame.
void Renderer_DrawTextDev(RayGE_Renderer* renderer, int posX, int posY, Color color, const char* text);
void Renderer_FormatTextDev(RayGE_Renderer* renderer, int posX, int posY, Color color, const char* format, ...);
