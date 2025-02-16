#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "RayGE/SceneTypes.h"
#include "Scene/Entity.h"
#include "wzl_cutl/attributes.h"

typedef struct RayGE_Scene RayGE_Scene;

WZL_ATTR_NODISCARD RayGE_Scene* Scene_Create(uint32_t maxEntities);
void Scene_Destroy(RayGE_Scene* scene);

uint32_t Scene_GetMaxEntities(const RayGE_Scene* scene);
uint32_t Scene_GetActiveEntities(const RayGE_Scene* scene);
RayGE_Entity* Scene_CreateEntity(RayGE_Scene* scene);
RayGE_Entity* Scene_GetActiveEntity(RayGE_Scene* scene, uint32_t index);
RayGE_Entity* Scene_GetEntityFromHandle(RayGE_Scene* scene, RayGE_ResourceHandle handle);
