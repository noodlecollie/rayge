#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Engine/EngineAPI.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Utils.h"
#include "Debugging.h"

typedef struct Scene
{
	RayGE_EntityList* entities;
} Scene;

static Scene* g_Scene = NULL;

static Scene* CreateScene(size_t maxEntities)
{
	Scene* scene = MEMPOOL_CALLOC_STRUCT(MEMPOOL_SCENE, Scene);
	scene->entities = Entity_AllocateList(RAYGE_MAX(maxEntities, 1));

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
		Entity_FreeList(scene->entities);
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
	return g_Scene ? Entity_GetListCapacity(g_Scene->entities) : 0;
}

size_t Scene_GetActiveEntities(void)
{
	return g_Scene ? Entity_GetNumUsedSlots(g_Scene->entities) : 0;
}

RayGE_Entity* Scene_CreateEntity(void)
{
	if ( !g_Scene )
	{
		return NULL;
	}

	if ( Entity_GetNumFreeSlots(g_Scene->entities) < 1 )
	{
		LoggingSubsystem_PrintLine(
			RAYGE_LOG_ERROR,
			"Max limit of %zu scene entities reached.",
			Entity_GetListCapacity(g_Scene->entities)
		);

		return NULL;
	}

	RayGE_Entity* ent = Entity_FindFirstFree(g_Scene->entities);

	RAYGE_ENSURE(
		ent,
		"Expected to be able to find a free entity in scene with only %zu of %zu slots filled.",
		Entity_GetNumFreeSlots(g_Scene->entities),
		Entity_GetListCapacity(g_Scene->entities)
	);

	Entity_Acquire(ent);

	return ent;
}

RayGE_Entity* Scene_GetActiveEntity(size_t index)
{
	if ( !g_Scene || index >= Entity_GetListCapacity(g_Scene->entities) )
	{
		return NULL;
	}

	RayGE_Entity* entity = Entity_Get(g_Scene->entities, index);
	return Entity_IsInUse(entity) ? entity : NULL;
}

RayGE_Entity* Scene_GetEntityFromHandle(RayGE_EntityHandle handle)
{
	return g_Scene ? Entity_GetFromHandle(g_Scene->entities, handle) : NULL;
}
