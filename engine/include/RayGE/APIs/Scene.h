// File: Scene.h
// Interface for scene and entity manipulation.

#pragma once

#include "RayGE/SceneTypes.h"

typedef struct RayGE_Scene_API
{
	RayGE_ResourceHandle (*CreateEntity)(void);
	RayGE_Component_Spatial* (*AddSpatialComponent)(RayGE_ResourceHandle entity);
	RayGE_Component_Spatial* (*GetSpatialComponent)(RayGE_ResourceHandle entity);
	RayGE_Component_Camera* (*AddCameraComponent)(RayGE_ResourceHandle entity);
	RayGE_Component_Camera* (*GetCameraComponent)(RayGE_ResourceHandle entity);
	RayGE_Component_Renderable* (*AddRenderableComponent)(RayGE_ResourceHandle entity);
	RayGE_Component_Renderable* (*GetRenderableComponent)(RayGE_ResourceHandle entity);
} RayGE_Scene_API;

typedef struct RayGE_Scene_Callbacks
{
	void (*SceneBegin)(void);
	void (*SceneEnd)(void);
} RayGE_Scene_Callbacks;
