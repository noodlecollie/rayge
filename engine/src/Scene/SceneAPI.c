#include "Scene/SceneAPI.h"
#include "Scene/Scene.h"
#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Subsystems/LoggingSubsystem.h"

static bool VerifyEntity(RayGE_Entity* entity, const char* operation)
{
	if ( !entity )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Cannot %s for null entity.", operation);
		return false;
	}

	if ( !Entity_IsInUse(entity) )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Cannot %s for entity that is not in use.", operation);
		return false;
	}

	return true;
}

RayGE_Entity* SceneAPI_CreateEntity(void)
{
	return Scene_CreateEntity();
}

RayGE_Component_Spatial* SceneAPI_AddSpatialComponent(RayGE_Entity* entity)
{
	if ( !VerifyEntity(entity, "add component") )
	{
		return NULL;
	}

	RayGE_ComponentHeader* existing = Entity_GetFirstComponentOfType(entity, RAYGE_COMPONENTTYPE_SPATIAL);

	if ( existing )
	{
		return COMPONENTDATA_SPATIAL(existing);
	}

	RayGE_ComponentImpl_Spatial* component = Component_CreateSpatial();

	if ( !Entity_AddComponent(entity, &component->header) )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Failed to add component to entity.");
		Component_FreeList(&component->header);
		component = NULL;
	}

	return &component->data;
}

RayGE_Component_Spatial* SceneAPI_GetSpatialComponent(RayGE_Entity* entity)
{
	if ( !VerifyEntity(entity, "get component") )
	{
		return NULL;
	}

	RayGE_ComponentHeader* component = Entity_GetFirstComponentOfType(entity, RAYGE_COMPONENTTYPE_SPATIAL);
	return component ? COMPONENTDATA_SPATIAL(component) : NULL;
}

RayGE_Component_Camera* SceneAPI_AddCameraComponent(RayGE_Entity* entity)
{
	if ( !VerifyEntity(entity, "add component") )
	{
		return NULL;
	}

	RayGE_ComponentHeader* existing = Entity_GetFirstComponentOfType(entity, RAYGE_COMPONENTTYPE_CAMERA);

	if ( existing )
	{
		return COMPONENTDATA_CAMERA(existing);
	}

	RayGE_ComponentImpl_Camera* component = Component_CreateCamera();

	if ( !Entity_AddComponent(entity, &component->header) )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Failed to add component to entity.");
		Component_FreeList(&component->header);
		component = NULL;
	}

	return &component->data;
}

RayGE_Component_Camera* SceneAPI_GetCameraComponent(RayGE_Entity* entity)
{
	if ( !VerifyEntity(entity, "get component") )
	{
		return NULL;
	}

	RayGE_ComponentHeader* component = Entity_GetFirstComponentOfType(entity, RAYGE_COMPONENTTYPE_CAMERA);
	return component ? COMPONENTDATA_CAMERA(component) : NULL;
}
