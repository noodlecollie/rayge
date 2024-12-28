#pragma once

#include "Scene/Scene.h"

void SceneSubsystem_Init(void);
void SceneSubsystem_ShutDown(void);

RayGE_Scene* SceneSubsystem_GetScene(void);
