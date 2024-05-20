#include <stdbool.h>
#include "Scene/Component.h"
#include "Modules/MemPoolModule.h"
#include "Debugging.h"

static void* CallocComponent(RayGE_ComponentType type, size_t size)
{
	RayGE_ComponentHeader* component = MEMPOOL_CALLOC(MEMPOOL_ENTITY, 1, size);
	component->type = type;
	return component;
}

#define CALLOC_COMPONENT(structType, componentType) ((structType*)CallocComponent((componentType), sizeof(structType)))

void Component_FreeList(RayGE_ComponentHeader* head)
{
	while ( head )
	{
		RayGE_ComponentHeader* next = head->next;
		MEMPOOL_FREE(head);
		head = next;
	}
}

RayGE_ComponentImpl_Spatial* Component_CreateSpatial(void)
{
	return CALLOC_COMPONENT(RayGE_ComponentImpl_Spatial, RAYGE_COMPONENTTYPE_SPATIAL);
}

RayGE_ComponentImpl_Camera* Component_CreateCamera(void)
{
	RayGE_ComponentImpl_Camera* component = CALLOC_COMPONENT(RayGE_ComponentImpl_Camera, RAYGE_COMPONENTTYPE_CAMERA);

	component->data.fieldOfView = 90.0f;

	return component;
}

RayGE_ComponentImpl_Renderable* Component_CreateRenderable(void)
{
	RayGE_ComponentImpl_Renderable* component =
		CALLOC_COMPONENT(RayGE_ComponentImpl_Renderable, RAYGE_COMPONENTTYPE_RENDERABLE);

	component->data.handle = RAYGE_INVALID_RESOURCE_HANDLE;
	component->data.color = (RayGE_Color){ 255, 255, 255, 255 };
	component->data.scale = 1.0f;

	return component;
}

void* Component_CastImpl(
	RayGE_ComponentHeader* header,
	RayGE_ComponentType toType,
	bool ensureTypeMatches,
	const char* file,
	int line
)
{
	if ( !header )
	{
		if ( ensureTypeMatches )
		{
			RAYGE_ENSURE(
				header,
				"Invocation from %s:%d: Provided component pointer was null.",
				file,
				line,
				header->type,
				toType
			);
		}

		return NULL;
	}

	if ( ensureTypeMatches )
	{
		RAYGE_ENSURE(
			header->type == toType,
			"Invocation from %s:%d: Component type %d did not match expected type %d",
			file,
			line,
			header->type,
			toType
		);
	}

	return header->type == toType ? (void*)header : NULL;
}
