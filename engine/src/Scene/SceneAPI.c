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
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "%s: entity handle was not valid.", operation);
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
	RayGE_Entity* entPtr = GetEntityFromHandle(entity, "AddSpatialComponent");

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
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "AddSpatialComponent: failed to add component to entity.");
		Component_FreeList(&component->header);
		component = NULL;
	}

	return &component->data;
}

RayGE_Component_Spatial* SceneAPI_GetSpatialComponent(RayGE_EntityHandle entity)
{
	RayGE_Entity* entPtr = GetEntityFromHandle(entity, "GetSpatialComponent");

	if ( !entPtr )
	{
		return NULL;
	}

	RayGE_ComponentHeader* component = Entity_GetFirstComponentOfType(entPtr, RAYGE_COMPONENTTYPE_SPATIAL);
	return component ? COMPONENTDATA_SPATIAL(component) : NULL;
}

RayGE_Component_Camera* SceneAPI_AddCameraComponent(RayGE_EntityHandle entity)
{
	RayGE_Entity* entPtr = GetEntityFromHandle(entity, "AddCameraComponent");

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
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "AddCameraComponent: failed to add component to entity.");
		Component_FreeList(&component->header);
		component = NULL;
	}

	return &component->data;
}

RayGE_Component_Camera* SceneAPI_GetCameraComponent(RayGE_EntityHandle entity)
{
	RayGE_Entity* entPtr = GetEntityFromHandle(entity, "GetCameraComponent");

	if ( !entPtr )
	{
		return NULL;
	}

	RayGE_ComponentHeader* component = Entity_GetFirstComponentOfType(entPtr, RAYGE_COMPONENTTYPE_CAMERA);
	return component ? COMPONENTDATA_CAMERA(component) : NULL;
}

RayGE_Component_Renderable* SceneAPI_AddRenderableComponent(RayGE_EntityHandle entity)
{
	RayGE_Entity* entPtr = GetEntityFromHandle(entity, "AddRenderableComponent");

	if ( !entPtr )
	{
		return NULL;
	}

	RayGE_ComponentHeader* existing = Entity_GetFirstComponentOfType(entPtr, RAYGE_COMPONENTTYPE_RENDERABLE);

	if ( existing )
	{
		return COMPONENTDATA_RENDERABLE(existing);
	}

	RayGE_ComponentImpl_Renderable* component = Component_CreateRenderable();

	if ( !Entity_AddComponent(entPtr, &component->header) )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "AddRenderableComponent: failed to add component to entity.");
		Component_FreeList(&component->header);
		component = NULL;
	}

	return &component->data;
}

RayGE_Component_Renderable* SceneAPI_GetRenderableComponent(RayGE_EntityHandle entity)
{
	RayGE_Entity* entPtr = GetEntityFromHandle(entity, "GetRenderableComponent");

	if ( !entPtr )
	{
		return NULL;
	}

	RayGE_ComponentHeader* component = Entity_GetFirstComponentOfType(entPtr, RAYGE_COMPONENTTYPE_RENDERABLE);
	return component ? COMPONENTDATA_RENDERABLE(component) : NULL;
}
