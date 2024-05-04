#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "RayGE/SceneTypes.h"

void Scene_CreateStatic(size_t maxEntities);
void Scene_DestroyStatic(void);
bool Scene_IsCreated(void);

size_t Scene_GetMaxEntities(void);
RayGE_Entity* Scene_CreateEntity(void);
RayGE_Entity* Scene_GetActiveEntity(size_t index);
