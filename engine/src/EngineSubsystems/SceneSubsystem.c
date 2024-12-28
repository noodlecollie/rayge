#include "EngineSubsystems/SceneSubsystem.h"
#include "Debugging.h"

static RayGE_Scene* g_Scene = NULL;

void SceneSubsystem_Init(void)
{
	if ( g_Scene )
	{
		return;
	}

	// TODO: Make this value configurable
	g_Scene = Scene_Create(1024);
}

void SceneSubsystem_ShutDown(void)
{
	if ( !g_Scene )
	{
		return;
	}

	Scene_Destroy(g_Scene);
	g_Scene = NULL;
}

RayGE_Scene* SceneSubsystem_GetScene(void)
{
	RAYGE_ASSERT_VALID(g_Scene);
	return g_Scene;
}
