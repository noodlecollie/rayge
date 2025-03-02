#pragma once

#include "RayGE/SceneTypes.h"
#include "Scene/Entity.h"
#include "raylib.h"
#include "wzl_cutl/attributes.h"

#define RENDERCOLOUR_NONE BLANK

typedef struct RayGE_Renderer RayGE_Renderer;

typedef enum Renderer_DebugFlag
{
	RENDERER_DBG_DRAW_LOCATIONS = (1 << 0),
	RENDERER_DBG_OVERRIDE_CAMERA = (1 << 1),
} Renderer_DebugFlag;

typedef uint16_t Renderer_Index;

typedef struct Renderer_RawVertex2D
{
	Vector2 pos;
	Vector2 uv;
	Color col;
} Renderer_RawVertex2D;

typedef struct Renderer_RawBatch2D
{
	const Renderer_RawVertex2D* vertices;
	size_t vertexCount;

	const Renderer_Index* indices;
	size_t indexCount;

	unsigned int textureID;
} Renderer_RawBatch2D;

WZL_ATTR_NODISCARD RayGE_Renderer* Renderer_Create(void);
void Renderer_Destroy(RayGE_Renderer* renderer);

void Renderer_AddDebugFlags(RayGE_Renderer* renderer, uint64_t flags);
void Renderer_RemoveDebugFlags(RayGE_Renderer* renderer, uint64_t flags);
void Renderer_SetDebugFlags(RayGE_Renderer* renderer, uint64_t flags);
void Renderer_ClearDebugFlags(RayGE_Renderer* renderer);
uint64_t Renderer_GetDebugFlags(const RayGE_Renderer* renderer);

void Renderer_SetBackgroundColour(RayGE_Renderer* renderer, Color colour);
void Renderer_ClearBackgroundColour(RayGE_Renderer* renderer);

Camera2D Renderer_GetDefaultCamera2D(void);
Camera3D Renderer_GetDefaultCamera3D(void);

Camera2D Renderer_GetDebugCamera2D(const RayGE_Renderer* renderer);
void Renderer_SetDebugCamera2D(RayGE_Renderer* renderer, Camera2D camera);
void Renderer_ResetDebugCamera2D(RayGE_Renderer* renderer);
Camera3D Renderer_GetDebugCamera3D(const RayGE_Renderer* renderer);
void Renderer_SetDebugCamera3D(RayGE_Renderer* renderer, Camera3D camera);
void Renderer_ResetDebugCamera3D(RayGE_Renderer* renderer);

// Any frame begins in direct drawing mode.
void Renderer_BeginFrame(RayGE_Renderer* renderer);
void Renderer_EndFrame(RayGE_Renderer* renderer);
bool Renderer_IsInFrame(const RayGE_Renderer* renderer);

// All functions below may only be called after a frame has begun.

// These functions will change the underlying rendering mode.
// Depending on what Raylib does underneath, this may be expensive
// as it may change the rendering context.
void Renderer_SetDrawingMode2D(RayGE_Renderer* renderer, Camera2D camera);
void Renderer_SetDrawingMode3D(RayGE_Renderer* renderer, Camera3D camera);
void Renderer_SetDrawingModeDirect(RayGE_Renderer* renderer);

void Renderer_DrawEntity3D(RayGE_Renderer* renderer, RayGE_Entity* entity);
void Renderer_DrawAllActiveEntitiesInScene3D(RayGE_Renderer* renderer);

// These functions require the renderer to be in direct drawing mode.
void Renderer_DirectDrawTextDev(RayGE_Renderer* renderer, int posX, int posY, Color color, const char* text);
void Renderer_DirectFormatTextDev(RayGE_Renderer* renderer, int posX, int posY, Color color, const char* format, ...);
void Renderer_DirectDrawImGui(RayGE_Renderer* renderer);

// Pass indexCount as 0 to draw everything, beginning from indexOffset.
void Renderer_DirectDrawTriangles2D(
	RayGE_Renderer* renderer,
	const Renderer_RawBatch2D* batch,
	size_t indexOffset,
	size_t indexCount
);
