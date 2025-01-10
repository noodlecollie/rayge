#pragma once

#include "RayGE/APIs/Renderable.h"

RayGE_ResourceHandle RenderableAPI_GetPrimitiveHandle(RayGE_RenderablePrimitive primitive);
RayGE_ResourceHandle RenderableAPI_LoadTexture(const char* path);
void RenderableAPI_UnloadTexture(RayGE_ResourceHandle handle);
