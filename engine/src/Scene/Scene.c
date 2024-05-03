#include "Scene/Scene.h"
#include "EngineAPI.h"

static bool g_SceneCreated = false;

void Scene_CreateStatic()
{
	g_SceneCreated = true;
	INVOKE_CALLBACK(g_GameLibCallbacks.scene.SceneBegin);
}

void Scene_DestroyStatic()
{
	INVOKE_CALLBACK(g_GameLibCallbacks.scene.SceneEnd);
	g_SceneCreated = false;
}

bool Scene_IsCreated()
{
	return g_SceneCreated;
}
