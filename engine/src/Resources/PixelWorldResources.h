#pragma once

#include "PixelWorld/PixelWorld.h"
#include "RayGE/ResourceHandle.h"
#include "wzl_cutl/attributes.h"

void PixelWorldResources_Init(void);
void PixelWorldResources_ShutDown(void);

WZL_ATTR_NODISCARD RayGE_ResourceHandle PixelWorldResources_LoadPixelWorld(const char* path);
void PixelWorldResources_UnloadPixelWorld(RayGE_ResourceHandle handle);
