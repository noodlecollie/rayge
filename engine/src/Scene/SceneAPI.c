#include "Scene/SceneAPI.h"
#include "Scene/Scene.h"
#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Subsystems/LoggingSubsystem.h"

static RayGE_Entity* GetEntityFromHandle(RayGE_EntityHandle handle, const char* operation)
{
	RayGE_Entity* entity = Scene_GetEntityFromHandle(handle);

	if ( !entity )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Cannot %s using invalid entity handle.", operation);
		return NULL;
	}

	if ( !Entity_IsInUse(entity) )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Cannot %s for entity that is not in use.", operation);
		return NULL;
	}

	return entity;
}

RayGE_EntityHandle SceneAPI_CreateEntity(void)
{
	return Entity_CreateHandle(Scene_CreateEntity());
}

RayGE_Component_Spatial* SceneAPI_AddSpatialComponent(RayGE_EntityHandle entity)
{
	RayGE_Entity* entPtr = GetEntityFromHandle(entity, "add component");

	if ( !entPtr )
	{
		return NULL;
	}

	RayGE_ComponentHeader* existing = Entity_GetFirstComponentOfType(entPtr, RAYGE_COMPONENTTYPE_SPATIAL);

	if ( existing )
	{
		return COMPONENTDATA_SPATIAL(existing);
	}

	RayGE_ComponentImpl_Spatial* component = Component_CreateSpatial();

	if ( !Entity_AddComponent(entPtr, &component->header) )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Failed to add component to entity.");
		Component_FreeList(&component->header);
		component = NULL;
	}

	return &component->data;
}

RayGE_Component_Spatial* SceneAPI_GetSpatialComponent(RayGE_EntityHandle entity)
{
	RayGE_Entity* entPtr = GetEntityFromHandle(entity, "get component");

	if ( !entPtr )
	{
		return NULL;
	}

	RayGE_ComponentHeader* component = Entity_GetFirstComponentOfType(entPtr, RAYGE_COMPONENTTYPE_SPATIAL);
	return component ? COMPONENTDATA_SPATIAL(component) : NULL;
}

RayGE_Component_Camera* SceneAPI_AddCameraComponent(RayGE_EntityHandle entity)
{
	RayGE_Entity* entPtr = GetEntityFromHandle(entity, "add component");

	if ( !entPtr )
	{
		return NULL;
	}

	RayGE_ComponentHeader* existing = Entity_GetFirstComponentOfType(entPtr, RAYGE_COMPONENTTYPE_CAMERA);

	if ( existing )
	{
		return COMPONENTDATA_CAMERA(existing);
	}

	RayGE_ComponentImpl_Camera* component = Component_CreateCamera();

	if ( !Entity_AddComponent(entPtr, &component->header) )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Failed to add component to entity.");
		Component_FreeList(&component->header);
		component = NULL;
	}

	return &component->data;
}

RayGE_Component_Camera* SceneAPI_GetCameraComponent(RayGE_EntityHandle entity)
{
	RayGE_Entity* entPtr = GetEntityFromHandle(entity, "get component");

	if ( !entPtr )
	{
		return NULL;
	}

	RayGE_ComponentHeader* component = Entity_GetFirstComponentOfType(entPtr, RAYGE_COMPONENTTYPE_CAMERA);
	return component ? COMPONENTDATA_CAMERA(component) : NULL;
}
