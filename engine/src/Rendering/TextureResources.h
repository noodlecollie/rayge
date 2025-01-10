#pragma once

#include "RayGE/ResourceHandle.h"

RayGE_ResourceHandle TextureResources_LoadTexture(const char* path);
void TextureResources_UnloadTexture(RayGE_ResourceHandle handle);
void TextureResources_UnloadAll(void);
