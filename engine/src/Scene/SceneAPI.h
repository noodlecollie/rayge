#pragma once

#include "RayGE/APIs/Scene.h"

RayGE_EntityHandle SceneAPI_CreateEntity(void);
RayGE_Component_Spatial* SceneAPI_AddSpatialComponent(RayGE_EntityHandle entity);
RayGE_Component_Spatial* SceneAPI_GetSpatialComponent(RayGE_EntityHandle entity);
RayGE_Component_Camera* SceneAPI_AddCameraComponent(RayGE_EntityHandle entity);
RayGE_Component_Camera* SceneAPI_GetCameraComponent(RayGE_EntityHandle entity);
RayGE_Component_Renderable* SceneAPI_AddRenderableComponent(RayGE_EntityHandle entity);
RayGE_Component_Renderable* SceneAPI_GetRenderableComponent(RayGE_EntityHandle entity);
