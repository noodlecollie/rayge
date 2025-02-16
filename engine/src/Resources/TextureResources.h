#pragma once

#include <stddef.h>
#include "RayGE/ResourceHandle.h"
#include "Resources/ResourceList.h"
#include "raylib.h"
#include "wzl_cutl/attributes.h"

void TextureResources_Init(void);
void TextureResources_ShutDown(void);

WZL_ATTR_NODISCARD RayGE_ResourceHandle TextureResources_LoadTexture(const char* path);
WZL_ATTR_NODISCARD RayGE_ResourceHandle TextureResources_LoadTextureAndRetainImage(const char* path, Image* outImage);
WZL_ATTR_NODISCARD RayGE_ResourceHandle TextureResources_LoadInternalTexture(const char* name, Image sourceImage);

// These two functions essentially do the same thing, but one of them is designed
// only to be called externally, and the other only to be called internally.
// This stops game clients from unloading internal resources.
void TextureResources_UnloadTexture(RayGE_ResourceHandle handle);
void TextureResources_UnloadInternalTexture(RayGE_ResourceHandle handle);

size_t TextureResources_NumTextures(void);

const ResourceList* TestureResources_GetResourceList(void);
Texture2D TextureResources_GetTexture(ResourceListIterator iterator);
const char* TextureResources_GetPath(ResourceListIterator iterator);
