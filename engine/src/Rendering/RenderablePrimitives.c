#include "Rendering/RenderablePrimitives.h"
#include "ResourceManagement/ResourceHandleUtils.h"

RayGE_ResourceHandle RenderablePrimitive_GetHandle(RayGE_RenderablePrimitive primitive)
{
	if ( primitive <= RAYGE_RENDERABLE_PRIM_INVALID || primitive >= RAYGE_RENDERABLE_PRIM__COUNT )
	{
		return RAYGE_NULL_RESOURCE_HANDLE;
	}

	// Keys don't matter here since these resources aren't specific items,
	// but more just constants representing primitives.
	return Resource_CreateInternalHandle(RESOURCE_DOMAIN_RENDERABLE_PRIMITIVE, (uint32_t)primitive, 0);
}

RayGE_RenderablePrimitive RenderablePrimitive_GetPrimitiveFromHandle(RayGE_ResourceHandle handle)
{
	if ( !Resource_HandleIsValidForInternalDomain(
			 handle,
			 RESOURCE_DOMAIN_RENDERABLE_PRIMITIVE,
			 (uint32_t)RAYGE_RENDERABLE_PRIM__COUNT
		 ) )
	{
		return RAYGE_RENDERABLE_PRIM_INVALID;
	}

	return (RayGE_RenderablePrimitive)handle.index;
}
