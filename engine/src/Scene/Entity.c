#include <string.h>
#include "Scene/Entity.h"
#include "Scene/Component.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Debugging.h"

struct RayGE_Entity
{
	bool isInUse;
	RayGE_ComponentHeader* componentsHead;
	RayGE_ComponentHeader* componentsTail;
	size_t componentCount;
};

RayGE_Entity* Entity_AllocateList(size_t count)
{
	RAYGE_ASSERT(count > 0, "Expected the entity count to be greater than zero.");

	if ( count < 1 )
	{
		return NULL;
	}

	return MEMPOOL_CALLOC(MEMPOOL_ENTITY, count, sizeof(RayGE_Entity));
}

void Entity_FreeList(RayGE_Entity* entities, size_t count)
{
	if ( !entities || count < 1 )
	{
		return;
	}

	for ( RayGE_Entity* entity = entities; count > 0; ++entity, --count )
	{
		if ( Entity_IsInUse(entity) )
		{
			Entity_Release(entity);
		}
	}

	MEMPOOL_FREE(entities);
}

void Entity_Acquire(RayGE_Entity* entity)
{
	if ( !entity )
	{
		return;
	}

	RAYGE_ASSERT(!entity->isInUse, "Entity was already in use");

	if ( entity->isInUse )
	{
		return;
	}

	memset(entity, 0, sizeof(*entity));
	entity->isInUse = true;
}

void Entity_Release(RayGE_Entity* entity)
{
	if ( !entity )
	{
		return;
	}

	RAYGE_ASSERT(entity->isInUse, "Entity was not in use");

	if ( !entity->isInUse )
	{
		return;
	}

	Component_FreeList(entity->componentsHead);

	entity->componentsHead = NULL;
	entity->componentsTail = NULL;
	entity->componentCount = 0;

	entity->isInUse = false;
}

bool Entity_IsInUse(RayGE_Entity* entity)
{
	return entity && entity->isInUse;
}

RayGE_Entity* Entity_FindFirstFree(RayGE_Entity* entities, size_t count)
{
	if ( !entities || count < 1 )
	{
		return NULL;
	}

	for ( size_t index = 0; index < count; ++index )
	{
		RayGE_Entity* entity = &entities[index];

		if ( !Entity_IsInUse(entity) )
		{
			return entity;
		}
	}

	return NULL;
}

bool Entity_AddComponent(RayGE_Entity* entity, RayGE_ComponentHeader* component)
{
	if ( !entity || !component )
	{
		return false;
	}

	RAYGE_ASSERT(Entity_IsInUse(entity), "Expected entity to be in use.");

	if ( !Entity_IsInUse(entity) )
	{
		return false;
	}

	if ( entity->componentsTail )
	{
		entity->componentsTail->next = component;
	}

	entity->componentsTail = component;

	if ( !entity->componentsHead )
	{
		entity->componentsHead = component;
	}

	++entity->componentCount;
	return true;
}

RayGE_ComponentHeader* Entity_GetFirstComponentOfType(RayGE_Entity* entity, RayGE_ComponentType type)
{
	if ( !entity )
	{
		return NULL;
	}

	for ( RayGE_ComponentHeader* component = entity->componentsHead; component; component = component->next )
	{
		if ( component->type == type )
		{
			return component;
		}
	}

	return NULL;
}
