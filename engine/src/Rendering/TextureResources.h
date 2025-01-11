#pragma once

#include "RayGE/ResourceHandle.h"
#include "raylib.h"

typedef struct TextureResources_Iterator TextureResources_Iterator;

RayGE_ResourceHandle TextureResources_LoadTexture(const char* path);
void TextureResources_UnloadTexture(RayGE_ResourceHandle handle);
void TextureResources_UnloadAll(void);
size_t TextureResources_NumTextures(void);

TextureResources_Iterator* TextureResources_CreateBeginIterator(void);
void TextureResources_DestroyIterator(TextureResources_Iterator* iterator);
bool TextureResources_IncrementIterator(TextureResources_Iterator* iterator);
Texture2D TextureResourcesIterator_GetTexture(TextureResources_Iterator* iterator);
const char* TextureResourcesIterator_GetPath(TextureResources_Iterator* iterator);
