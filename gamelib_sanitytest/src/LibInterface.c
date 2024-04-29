#include <stdio.h>
#include "GameLib_SanityTest/LibInterface.h"

GAMELIB_SANITYTEST_PUBLIC(void) RayGE_GameLibrary_Startup(RayGE_Engine_GetAPIFunc getEngineAPIFunc)
{
	if ( !getEngineAPIFunc )
	{
		fprintf(stderr, "getEngineAPIFunc was not provided\n");
		return;
	}

	uint16_t actualVersion = 0;
	const RayGE_Engine_API_V1* api = getEngineAPIFunc(RAYGE_ENGINEAPI_VERSION_1, &actualVersion);

	if ( !api )
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
	api->log.printLine(RAYGE_LOG_INFO, "Sanity test loaded RayGE API successfully.");
}
