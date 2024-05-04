#pragma once

#include "RayGE/APIs/Scene.h"

RayGE_Entity* SceneAPI_CreateEntity(void);
RayGE_Component_Spatial* SceneAPI_AddSpatialComponent(RayGE_Entity* entity);
RayGE_Component_Spatial* SceneAPI_GetSpatialComponent(RayGE_Entity* entity);
RayGE_Component_Camera* SceneAPI_AddCameraComponent(RayGE_Entity* entity);
RayGE_Component_Camera* SceneAPI_GetCameraComponent(RayGE_Entity* entity);
