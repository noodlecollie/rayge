// File: Scene.h
// Interface for scene and entity manipulation.

#pragma once

#include "RayGE/SceneTypes.h"

typedef struct RayGE_Scene_API
{
	RayGE_Entity* (*CreateEntity)(RayGE_Entity* parent);
	RayGE_Component_Spatial* (*AddSpatialComponent)(RayGE_Entity* entity);
	RayGE_Component_Spatial* (*GetSpatialComponent)(RayGE_Entity* entity);
	RayGE_Component_Camera* (*AddCameraComponent)(RayGE_Entity* entity);
	RayGE_Component_Camera* (*GetCameraComponent)(RayGE_Entity* entity);
} RayGE_Scene_API;
