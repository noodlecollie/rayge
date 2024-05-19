#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "RayGE/SceneTypes.h"
#include "Scene/Entity.h"

// TODO: Structify this, and move it to a module for lifetime management

void Scene_CreateStatic(size_t maxEntities);
void Scene_DestroyStatic(void);
bool Scene_IsCreated(void);

size_t Scene_GetMaxEntities(void);
size_t Scene_GetActiveEntities(void);
RayGE_Entity* Scene_CreateEntity(void);
RayGE_Entity* Scene_GetActiveEntity(size_t index);
RayGE_Entity* Scene_GetEntityFromHandle(RayGE_EntityHandle handle);
