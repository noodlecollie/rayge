#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "RayGE/SceneTypes.h"
#include "Scene/Entity.h"

typedef struct RayGE_Scene RayGE_Scene;

RayGE_Scene* Scene_Create(size_t maxEntities);
void Scene_Destroy(RayGE_Scene* scene);

size_t Scene_GetMaxEntities(const RayGE_Scene* scene);
size_t Scene_GetActiveEntities(const RayGE_Scene* scene);
RayGE_Entity* Scene_CreateEntity(RayGE_Scene* scene);
RayGE_Entity* Scene_GetActiveEntity(RayGE_Scene* scene, size_t index);
RayGE_Entity* Scene_GetEntityFromHandle(RayGE_Scene* scene, RayGE_EntityHandle handle);
