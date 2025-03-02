#include <string.h>
#include "Scene/Entity.h"
#include "Scene/Component.h"
#include "MemPool/MemPoolManager.h"
#include "Resources/ResourceHandleUtils.h"
#include "Debugging.h"
#include "Resources/ResourceHandleUtils.h"

struct RayGE_Entity
{
	RayGE_EntityList* parentList;
	uint32_t indexInParent;
	bool isInUse;
	RayGE_ComponentHeader* componentsHead;
	RayGE_ComponentHeader* componentsTail;
	size_t componentCount;
	uint64_t key;
};

struct RayGE_EntityList
{
	RayGE_Entity* entities;
	uint32_t capacity;
	uint32_t numInUse;
};

RayGE_EntityList* Entity_AllocateList(uint32_t capacity)
{
	RAYGE_ASSERT(capacity > 0, "Expected the entity list capacity to be greater than zero.");

	if ( capacity < 1 )
	{
		return NULL;
	}

	RayGE_EntityList* list = MEMPOOL_CALLOC_STRUCT(MEMPOOL_ENTITY, RayGE_EntityList);

	list->capacity = capacity;
	list->entities = (RayGE_Entity*)MEMPOOL_CALLOC(MEMPOOL_ENTITY, list->capacity, sizeof(RayGE_Entity));

	for ( uint32_t index = 0; index < list->capacity; ++index )
	{
		RayGE_Entity* entity = &list->entities[index];

		entity->parentList = list;
		entity->indexInParent = index;
		entity->key = Resource_CreateKey(index);
	}

	return list;
}

void Entity_FreeList(RayGE_EntityList* list)
{
	if ( !list )
	{
		return;
	}

	if ( list->entities )
	{
		for ( RayGE_Entity* entity = list->entities; list->capacity > 0; ++entity, --list->capacity )
		{
			if ( Entity_IsInUse(entity) )
			{
				Entity_Release(entity);
			}
		}

		MEMPOOL_FREE(list->entities);
	}

	MEMPOOL_FREE(list);
}

uint32_t Entity_GetListCapacity(const RayGE_EntityList* list)
{
	return list ? list->capacity : 0;
}

uint32_t Entity_GetNumFreeSlots(const RayGE_EntityList* list)
{
	if ( !list )
	{
		return 0;
	}

	RAYGE_ASSERT(list->numInUse <= list->capacity, "Expected number of in-use entities to be within list capacity");

	return list->capacity - list->numInUse;
}

uint32_t Entity_GetNumUsedSlots(const RayGE_EntityList* list)
{
	return list ? list->numInUse : 0;
}

RayGE_Entity* Entity_Get(const RayGE_EntityList* list, uint32_t index)
{
	if ( !list || !list->entities || index >= list->capacity )
	{
		return NULL;
	}

	return &list->entities[index];
}

RayGE_Entity* Entity_FindFirstFree(const RayGE_EntityList* list)
{
	if ( !list || !list->entities || list->capacity < 1 )
	{
		return NULL;
	}

	for ( uint32_t index = 0; index < list->capacity; ++index )
	{
		RayGE_Entity* entity = &list->entities[index];

		if ( !Entity_IsInUse(entity) )
		{
			return entity;
		}
	}

	return NULL;
}

RayGE_ResourceHandle Entity_CreateHandle(const RayGE_Entity* entity)
{
	if ( !entity )
	{
		return RAYGE_NULL_RESOURCE_HANDLE;
	}

	return Resource_CreateInternalHandle(RESOURCE_DOMAIN_ENTITY, entity->indexInParent, entity->key);
}

RayGE_Entity* Entity_GetFromHandle(const RayGE_EntityList* list, RayGE_ResourceHandle handle)
{
	RAYGE_ASSERT_VALID(list);

	if ( !list )
	{
		return NULL;
	}

	if ( !Resource_HandleIsValidForInternalDomain(handle, RESOURCE_DOMAIN_ENTITY, list->capacity) )
	{
		return NULL;
	}

	RayGE_Entity* entity = Entity_Get(list, handle.index);
	return (entity && entity->isInUse && entity->key == handle.key) ? entity : NULL;
}

void Entity_Acquire(RayGE_Entity* entity)
{
	if ( !entity )
	{
		return;
	}

	RAYGE_ASSERT(!entity->isInUse, "Entity was already in use");
	RAYGE_ASSERT(entity->parentList, "Expected valid parent list for entity");

	if ( entity->isInUse || !entity->parentList )
	{
		return;
	}

	entity->isInUse = true;

	++entity->parentList->numInUse;
}

void Entity_Release(RayGE_Entity* entity)
{
	if ( !entity )
	{
		return;
	}

	RAYGE_ASSERT(entity->isInUse, "Entity was not in use");
	RAYGE_ASSERT(entity->parentList, "Expected valid parent list for entity");

	if ( !entity->isInUse || !entity->parentList )
	{
		return;
	}

	// Something's gone very wrong if this is not true:
	RAYGE_ENSURE(entity->parentList->numInUse > 0, "Tried to release in-use entity that was not correctly recorded");

	Component_FreeList(entity->componentsHead);
	entity->componentsHead = NULL;
	entity->componentsTail = NULL;
	entity->componentCount = 0;

	entity->isInUse = false;

	// Reset the key to make sure that entity handles referring to
	// this index will no longer pass.
	entity->key = Resource_CreateKey(entity->indexInParent);

	--entity->parentList->numInUse;
}

bool Entity_IsInUse(const RayGE_Entity* entity)
{
	return entity && entity->isInUse;
}

uint32_t Entity_GetIndex(const RayGE_Entity* entity)
{
	RAYGE_ASSERT(entity, "Cannot get index of null entity.");
	return entity ? entity->indexInParent : UINT32_MAX;
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

RayGE_ComponentHeader* Entity_GetFirstComponentOfType(const RayGE_Entity* entity, RayGE_ComponentType type)
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
