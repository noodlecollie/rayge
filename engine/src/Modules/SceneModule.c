#include "Modules/SceneModule.h"
#include "Debugging.h"

static RayGE_Scene* g_Scene = NULL;

void SceneModule_Init(void)
{
	if ( g_Scene )
	{
		return;
	}

	// TODO: Make this value configurable
	g_Scene = Scene_Create(1024);
}

void SceneModule_ShutDown(void)
{
	if ( !g_Scene )
	{
		return;
	}

	Scene_Destroy(g_Scene);
	g_Scene = NULL;
}

RayGE_Scene* SceneModule_GetScene(void)
{
	RAYGE_ASSERT_VALID(g_Scene);
	return g_Scene;
}
