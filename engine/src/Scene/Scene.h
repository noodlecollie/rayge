#pragma once

#include <stdbool.h>
#include <stddef.h>

void Scene_CreateStatic(size_t maxEntities);
void Scene_DestroyStatic(void);
bool Scene_IsCreated(void);
size_t Scene_GetMaxEntities(void);
