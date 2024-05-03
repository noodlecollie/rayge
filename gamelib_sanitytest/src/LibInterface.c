#include <stdio.h>
#include "GameLib_SanityTest/LibInterface.h"

static void Game_StartUp(void);
static void Game_ShutDown(void);

static const RayGE_Engine_API_V1* g_EngineAPI = NULL;

static const RayGE_GameLib_Callbacks_V1 g_Callbacks = {
	// Game
	{
		Game_StartUp,
		Game_ShutDown,
	}
};

static void Game_StartUp(void)
{
	g_EngineAPI->log.printLine(RAYGE_LOG_INFO, "Sanity test: Game_StartUp()");
}

static void Game_ShutDown(void)
{
	g_EngineAPI->log.printLine(RAYGE_LOG_INFO, "Sanity test: Game_ShutDown()");
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
	g_EngineAPI->log.printLine(RAYGE_LOG_INFO, "Sanity test loaded RayGE API successfully.");
}
