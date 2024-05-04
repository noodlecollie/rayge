#pragma once

#include <stdbool.h>
#include "Scene/Component.h"

RayGE_Entity* Entity_AllocateList(size_t count);
void Entity_FreeList(RayGE_Entity* entities, size_t count);
RayGE_Entity* Entity_Get(RayGE_Entity* entities, size_t count, size_t index);

void Entity_Acquire(RayGE_Entity* entity);
void Entity_Release(RayGE_Entity* entity);
bool Entity_IsInUse(RayGE_Entity* entity);
RayGE_Entity* Entity_FindFirstFree(RayGE_Entity* entities, size_t count);

bool Entity_AddComponent(RayGE_Entity* entity, RayGE_ComponentHeader* component);
RayGE_ComponentHeader* Entity_GetFirstComponentOfType(RayGE_Entity* entity, RayGE_ComponentType type);
