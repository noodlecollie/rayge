#include "Rendering/RenderableAPI.h"
#include "Rendering/RenderablePrimitives.h"

RayGE_ResourceHandle RenderableAPI_GetPrimitiveHandle(RayGE_RenderablePrimitive primitive)
{
	return RenderablePrimitive_GetHandle(primitive);
}
