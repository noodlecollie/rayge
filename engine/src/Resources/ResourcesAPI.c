#include "Resources/ResourcesAPI.h"
#include "Resources/RenderablePrimitives.h"
#include "Resources/TextureResources.h"

RayGE_ResourceHandle ResourcesAPI_GetPrimitiveHandle(RayGE_RenderablePrimitive primitive)
{
	return RenderablePrimitive_GetHandle(primitive);
}

RayGE_ResourceHandle ResourcesAPI_LoadTexture(const char* path)
{
	return TextureResources_LoadTexture(path);
}

void ResourcesAPI_UnloadTexture(RayGE_ResourceHandle handle)
{
	TextureResources_UnloadTexture(handle);
}
