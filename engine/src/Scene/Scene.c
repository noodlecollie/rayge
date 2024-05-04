#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Engine/EngineAPI.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Utils.h"

typedef struct Scene
{
	size_t maxEntities;
	RayGE_Entity* entities;
} Scene;

static Scene* g_Scene = NULL;

static Scene* CreateScene(size_t maxEntities)
{
	Scene* scene = MEMPOOL_CALLOC_STRUCT(MEMPOOL_SCENE, Scene);
	scene->maxEntities = RAYGE_MAX(maxEntities, 1);
	scene->entities = MEMPOOL_CALLOC(MEMPOOL_SCENE, scene->maxEntities, sizeof(RayGE_Entity));

	return scene;
}

static void DestroyScene(Scene* scene)
{
	if ( !scene )
	{
		return;
	}

	if ( scene->entities )
	{
		MEMPOOL_FREE(scene->entities);
	}

	MEMPOOL_FREE(scene);
}

void Scene_CreateStatic(size_t maxEntities)
{
	if ( g_Scene )
	{
		return;
	}

	g_Scene = CreateScene(maxEntities);
	INVOKE_CALLBACK(g_GameLibCallbacks.scene.SceneBegin);
}

void Scene_DestroyStatic(void)
{
	if ( !g_Scene )
	{
		return;
	}

	INVOKE_CALLBACK(g_GameLibCallbacks.scene.SceneEnd);
	DestroyScene(g_Scene);
	g_Scene = NULL;
}

bool Scene_IsCreated(void)
{
	return g_Scene != NULL;
}

size_t Scene_GetMaxEntities(void)
{
	return g_Scene ? g_Scene->maxEntities : 0;
}
