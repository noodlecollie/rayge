#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Engine/EngineAPI.h"
#include "Modules/MemPoolModule.h"
#include "Logging/Logging.h"
#include "Utils.h"
#include "Debugging.h"

struct RayGE_Scene
{
	RayGE_EntityList* entities;
};

RayGE_Scene* Scene_Create(uint32_t maxEntities)
{
	RayGE_Scene* scene = MEMPOOL_CALLOC_STRUCT(MEMPOOL_SCENE, RayGE_Scene);
	scene->entities = Entity_AllocateList(RAYGE_MAX(maxEntities, 1));

	return scene;
}

void Scene_Destroy(RayGE_Scene* scene)
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

uint32_t Scene_GetMaxEntities(const RayGE_Scene* scene)
{
	RAYGE_ASSERT_VALID(scene);
	return scene ? Entity_GetListCapacity(scene->entities) : 0;
}

uint32_t Scene_GetActiveEntities(const RayGE_Scene* scene)
{
	RAYGE_ASSERT_VALID(scene);
	return scene ? Entity_GetNumUsedSlots(scene->entities) : 0;
}

RayGE_Entity* Scene_CreateEntity(RayGE_Scene* scene)
{
	RAYGE_ASSERT_VALID(scene);

	if ( !scene )
	{
		return NULL;
	}

	if ( Entity_GetNumFreeSlots(scene->entities) < 1 )
	{
		Logging_PrintLine(
			RAYGE_LOG_ERROR,
			"Max limit of %zu scene entities reached.",
			Entity_GetListCapacity(scene->entities)
		);

		return NULL;
	}

	RayGE_Entity* ent = Entity_FindFirstFree(scene->entities);

	RAYGE_ENSURE(
		ent,
		"Expected to be able to find a free entity in scene with only %zu of %zu slots filled.",
		Entity_GetNumFreeSlots(scene->entities),
		Entity_GetListCapacity(scene->entities)
	);

	Entity_Acquire(ent);

	return ent;
}

RayGE_Entity* Scene_GetActiveEntity(RayGE_Scene* scene, uint32_t index)
{
	RAYGE_ASSERT_VALID(scene);

	if ( !scene || index >= Entity_GetListCapacity(scene->entities) )
	{
		return NULL;
	}

	RayGE_Entity* entity = Entity_Get(scene->entities, index);
	return Entity_IsInUse(entity) ? entity : NULL;
}

RayGE_Entity* Scene_GetEntityFromHandle(RayGE_Scene* scene, RayGE_ResourceHandle handle)
{
	RAYGE_ASSERT_VALID(scene);
	return scene ? Entity_GetFromHandle(scene->entities, handle) : NULL;
}
