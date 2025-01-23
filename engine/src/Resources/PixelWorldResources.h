#pragma once

#include "PixelWorld/PixelWorld.h"
#include "RayGE/ResourceHandle.h"

void PixelWorldResources_Init(void);
void PixelWorldResources_ShutDown(void);

RayGE_ResourceHandle PixelWorldResources_LoadPixelWorld(const char* path);
void PixelWorldResources_UnloadPixelWorld(RayGE_ResourceHandle handle);
