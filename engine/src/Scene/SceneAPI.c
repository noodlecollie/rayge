#include "Scene/SceneAPI.h"
#include "Subsystems/LoggingSubsystem.h"

RayGE_Entity* SceneAPI_CreateEntity(RayGE_Entity* parent)
{
	(void)parent;
	LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "SceneAPI_CreateEntity is unimplemented");
	return NULL;
}

RayGE_Component_Spatial* SceneAPI_AddSpatialComponent(RayGE_Entity* entity)
{
	(void)entity;
	LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "SceneAPI_AddSpatialComponent is unimplemented");
	return NULL;
}

RayGE_Component_Spatial* SceneAPI_GetSpatialComponent(RayGE_Entity* entity)
{
	(void)entity;
	LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "SceneAPI_GetSpatialComponent is unimplemented");
	return NULL;
}

RayGE_Component_Camera* SceneAPI_AddCameraComponent(RayGE_Entity* entity)
{
	(void)entity;
	LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "SceneAPI_AddCameraComponent is unimplemented");
	return NULL;
}

RayGE_Component_Camera* SceneAPI_GetCameraComponent(RayGE_Entity* entity)
{
	(void)entity;
	LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "SceneAPI_GetCameraComponent is unimplemented");
	return NULL;
}
