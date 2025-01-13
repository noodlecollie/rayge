#pragma once

#include "RayGE/ResourceHandle.h"
#include "raylib.h"

typedef struct TextureResources_Iterator
{
	size_t batchIndex;
	size_t entryIndex;
} TextureResources_Iterator;

RayGE_ResourceHandle TextureResources_LoadTexture(const char* path);
RayGE_ResourceHandle TextureResources_LoadInternalTexture(const char* name, Image sourceImage);

// These two functions essentially do the same thing, but one of them is designed
// only to be called externally, and the other only to be called internally.
// This stops game clients from unloading internal resources.
void TextureResources_UnloadTexture(RayGE_ResourceHandle handle);
void TextureResources_UnloadInternalTexture(RayGE_ResourceHandle handle);

void TextureResources_UnloadAll(void);
size_t TextureResources_NumTextures(void);

TextureResources_Iterator TextureResources_CreateBeginIterator(void);
TextureResources_Iterator TextureResources_CreateIterator(RayGE_ResourceHandle handle);
bool TextureResources_IsIteratorValid(TextureResources_Iterator);
bool TextureResources_IncrementIterator(TextureResources_Iterator* iterator);
Texture2D TextureResourcesIterator_GetTexture(TextureResources_Iterator iterator);
const char* TextureResourcesIterator_GetPath(TextureResources_Iterator iterator);
