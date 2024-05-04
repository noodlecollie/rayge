#include <stdbool.h>
#include "Scene/Component.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Debugging.h"

static void* CallocComponent(RayGE_ComponentType type, size_t size)
{
	RayGE_ComponentHeader* component = MEMPOOL_CALLOC(MEMPOOL_ENTITY, 1, size);
	component->type = type;
	return component;
}

#define CALLOC_COMPONENT(structType, componentType) ((structType*)CallocComponent((componentType), sizeof(structType)))

void RayGE_Component_FreeList(RayGE_ComponentHeader* head)
{
	while ( head )
	{
		RayGE_ComponentHeader* next = head->next;
		MEMPOOL_FREE(head);
		head = next;
	}
}

RayGE_ComponentImpl_Spatial* RayGE_Component_CreateSpatial(void)
{
	return CALLOC_COMPONENT(RayGE_ComponentImpl_Spatial, RAYGE_COMPONENTTYPE_SPATIAL);
}

RayGE_ComponentImpl_Camera* RayGE_Component_CreateCamera(void)
{
	return CALLOC_COMPONENT(RayGE_ComponentImpl_Camera, RAYGE_COMPONENTTYPE_CAMERA);
}

void* RayGE_Component_CastImpl(
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
