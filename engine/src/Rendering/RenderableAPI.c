#include "Rendering/RenderableAPI.h"
#include "Resources/RenderablePrimitives.h"
#include "Resources/TextureResources.h"

RayGE_ResourceHandle RenderableAPI_GetPrimitiveHandle(RayGE_RenderablePrimitive primitive)
{
	return RenderablePrimitive_GetHandle(primitive);
}

RayGE_ResourceHandle RenderableAPI_LoadTexture(const char* path)
{
	return TextureResources_LoadTexture(path);
}

void RenderableAPI_UnloadTexture(RayGE_ResourceHandle handle)
{
	TextureResources_UnloadTexture(handle);
}
