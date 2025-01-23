// File: Resources.h
// Interface for loading and unloading resources for use in a game.

#pragma once

#include "RayGE/ResourceHandle.h"

typedef enum RayGE_RenderablePrimitive
{
	RAYGE_RENDERABLE_PRIM_INVALID = 0,
	RAYGE_RENDERABLE_PRIM_SPHERE,
	RAYGE_RENDERABLE_PRIM_AACUBE,

	RAYGE_RENDERABLE_PRIM__COUNT
} RayGE_RenderablePrimitive;

typedef struct RayGE_Resources_API
{
	RayGE_ResourceHandle (*GetPrimitiveHandle)(RayGE_RenderablePrimitive primitive);
	RayGE_ResourceHandle (*LoadTexture)(const char* path);
	void (*UnloadTexture)(RayGE_ResourceHandle handle);
	RayGE_ResourceHandle (*LoadPixelWorld)(const char* path);
	void (*UnloadPixelWorld)(RayGE_ResourceHandle handle);
} RayGE_Resources_API;
