#include <stdio.h>
#include "LibExport.h"
#include "RayGE/APIs/Engine.h"

static void Game_StartUp(void);
static void Game_ShutDown(void);
static void Scene_Begin(void);
static void Scene_End(void);

static const RayGE_Engine_API_V1* g_EngineAPI = NULL;
static const RayGE_GameLib_Callbacks_V1 g_Callbacks = {
	// Game
	{
		Game_StartUp,
		Game_ShutDown,
		NULL,
		NULL
	},

	// Scene
	{
		Scene_Begin,
		Scene_End,
	}
};

static RayGE_Entity* g_SubjectEntity = NULL;
static RayGE_Entity* g_CameraEntity = NULL;

static void Game_StartUp(void)
{
	g_EngineAPI->log.PrintLine(RAYGE_LOG_INFO, "Sanity test: Game_StartUp()");
}

static void Game_ShutDown(void)
{
	g_EngineAPI->log.PrintLine(RAYGE_LOG_INFO, "Sanity test: Game_ShutDown()");
}

static void Scene_Begin(void)
{
	g_EngineAPI->log.PrintLine(RAYGE_LOG_INFO, "Sanity test: Scene_Begin()");

	g_EngineAPI->log.PrintLine(RAYGE_LOG_INFO, "Sanity test: Adding subject entity");
	g_SubjectEntity = g_EngineAPI->scene.CreateEntity();

	RayGE_Component_Spatial* spatial = g_EngineAPI->scene.AddSpatialComponent(g_SubjectEntity);
	spatial->position.x = -10.0f;
	spatial->angles.pitch = 0.0f;

	g_EngineAPI->log.PrintLine(RAYGE_LOG_INFO, "Sanity test: Adding camera entity");
	g_CameraEntity = g_EngineAPI->scene.CreateEntity();

	RayGE_Component_Spatial* cameraPos = g_EngineAPI->scene.AddSpatialComponent(g_CameraEntity);
	cameraPos->position.x = 0.0f;
	cameraPos->angles.pitch = -20.0f;
	cameraPos->angles.roll = 0.0f;
	cameraPos->angles.yaw = 10.0f;

	RayGE_Component_Camera* camera = g_EngineAPI->scene.AddCameraComponent(g_CameraEntity);
	camera->fieldOfView = 80.0f;
}

static void Scene_End(void)
{
	g_EngineAPI->log.PrintLine(RAYGE_LOG_INFO, "Sanity test: Scene_End()");
}

GAMELIB_SANITYTEST_PUBLIC(void) RayGE_GameLibrary_ExchangeAPIs(RayGE_Engine_GetAPIFunc getEngineAPIFunc)
{
	if ( !getEngineAPIFunc )
	{
		fprintf(stderr, "getEngineAPIFunc was not provided\n");
		return;
	}

	uint16_t actualVersion = 0;
	g_EngineAPI = getEngineAPIFunc(RAYGE_ENGINEAPI_VERSION_1, &g_Callbacks, &actualVersion);

	if ( !g_EngineAPI )
	{
		fprintf(
			stderr,
			"Could not get RayGE engine API version %u (got version %u)\n",
			RAYGE_ENGINEAPI_VERSION_1,
			actualVersion
		);

		return;
	}

	// We now know that the API is safe to use and matches what we expect.
	g_EngineAPI->log.PrintLine(RAYGE_LOG_INFO, "Sanity test loaded RayGE API successfully.");
}
