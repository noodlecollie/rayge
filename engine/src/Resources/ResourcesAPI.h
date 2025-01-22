#pragma once

#include "RayGE/APIs/Resources.h"

RayGE_ResourceHandle ResourcesAPI_GetPrimitiveHandle(RayGE_RenderablePrimitive primitive);
RayGE_ResourceHandle ResourcesAPI_LoadTexture(const char* path);
void ResourcesAPI_UnloadTexture(RayGE_ResourceHandle handle);
