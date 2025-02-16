#pragma once

#include "RayGE/APIs/Resources.h"
#include "wzl_cutl/attributes.h"

RayGE_ResourceHandle ResourcesAPI_GetPrimitiveHandle(RayGE_RenderablePrimitive primitive);
WZL_ATTR_NODISCARD RayGE_ResourceHandle ResourcesAPI_LoadTexture(const char* path);
void ResourcesAPI_UnloadTexture(RayGE_ResourceHandle handle);
WZL_ATTR_NODISCARD RayGE_ResourceHandle ResourcesAPI_LoadPixelWorld(const char* path);
void ResourcesAPI_UnloadPixelWorld(RayGE_ResourceHandle handle);
