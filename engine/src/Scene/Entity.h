#pragma once

#include <stdbool.h>
#include "RayGE/SceneTypes.h"
#include "RayGE/ResourceHandle.h"
#include "Scene/Component.h"

typedef struct RayGE_Entity RayGE_Entity;
typedef struct RayGE_EntityList RayGE_EntityList;

RayGE_EntityList* Entity_AllocateList(uint32_t capacity);
void Entity_FreeList(RayGE_EntityList* list);
uint32_t Entity_GetListCapacity(const RayGE_EntityList* list);
uint32_t Entity_GetNumFreeSlots(const RayGE_EntityList* list);
uint32_t Entity_GetNumUsedSlots(const RayGE_EntityList* list);
RayGE_Entity* Entity_Get(const RayGE_EntityList* list, uint32_t index);
RayGE_Entity* Entity_FindFirstFree(const RayGE_EntityList* list);

RayGE_ResourceHandle Entity_CreateHandle(const RayGE_Entity* entity);

// Entity must be in use, and the key must match.
RayGE_Entity* Entity_GetFromHandle(const RayGE_EntityList* list, RayGE_ResourceHandle handle);

void Entity_Acquire(RayGE_Entity* entity);
void Entity_Release(RayGE_Entity* entity);
bool Entity_IsInUse(const RayGE_Entity* entity);
uint32_t Entity_GetIndex(const RayGE_Entity* entity);

bool Entity_AddComponent(RayGE_Entity* entity, RayGE_ComponentHeader* component);
RayGE_ComponentHeader* Entity_GetFirstComponentOfType(const RayGE_Entity* entity, RayGE_ComponentType type);
