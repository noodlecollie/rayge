#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Engine/EngineAPI.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Utils.h"
#include "Debugging.h"

typedef struct Scene
{
	size_t maxEntities;
	size_t entityCount;
	RayGE_Entity* entities;
} Scene;

static Scene* g_Scene = NULL;

static Scene* CreateScene(size_t maxEntities)
{
	Scene* scene = MEMPOOL_CALLOC_STRUCT(MEMPOOL_SCENE, Scene);
	scene->maxEntities = RAYGE_MAX(maxEntities, 1);
	scene->entities = Entity_AllocateList(scene->maxEntities);

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
		Entity_FreeList(scene->entities, scene->maxEntities);
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

RayGE_Entity* Scene_CreateEntity(void)
{
	if ( !g_Scene )
	{
		return NULL;
	}

	if ( g_Scene->entityCount >= g_Scene->maxEntities )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Max limit of %zu scene entities reached.", g_Scene->maxEntities);
		return NULL;
	}

	RayGE_Entity* ent = Entity_FindFirstFree(g_Scene->entities, g_Scene->maxEntities);

	RAYGE_ENSURE(
		ent,
		"Expected to be able to find a free entity in scene with only %zu of %zu slots filled.",
		g_Scene->entityCount,
		g_Scene->maxEntities
	);

	Entity_Acquire(ent);
	++g_Scene->entityCount;

	return ent;
}

RayGE_Entity* Scene_GetActiveEntity(size_t index)
{
	if ( !g_Scene || index >= g_Scene->maxEntities )
	{
		return NULL;
	}

	RayGE_Entity* entity = Entity_Get(g_Scene->entities, g_Scene->maxEntities, index);
	return Entity_IsInUse(entity) ? entity : NULL;
}
