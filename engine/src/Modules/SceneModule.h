#pragma once

#include "Scene/Scene.h"

void SceneModule_Init(void);
void SceneModule_ShutDown(void);

RayGE_Scene* SceneModule_GetScene(void);
