#include <stddef.h>
#include "GameLoader.h"
#include "wzl_cutl/libloader.h"

static const RayGE_Engine_API* RAYGE_ENGINE_CDECL GetEngineAPI(
	uint16_t requestedVersion,
	uint16_t* outSupportedVersion
)
{
	// TODO: Implement
	(void)requestedVersion;

	if ( outSupportedVersion )
	{
		*outSupportedVersion = 0;
	}

	return NULL;
}

bool GameLoader_InvokeGameLibraryStartup(void* gameLibrary)
{
	if ( !gameLibrary )
	{
		return false;
	}

	GameLibFunc_Startup startupFunc =
		(GameLibFunc_Startup)wzl_get_library_function(gameLibrary, RAYGE_GAMELIBRARY_STARTUP_SYMBOL_NAME);

	if ( !startupFunc )
	{
		return false;
	}

	startupFunc(&GetEngineAPI);
	return true;
}

void GameLoader_InvokeGameLibraryShutdown(void* gameLibrary)
{
	if ( !gameLibrary )
	{
		return;
	}

	GameLibFunc_ShutDown shutdownFunc =
		(GameLibFunc_ShutDown)wzl_get_library_function(gameLibrary, RAYGE_GAMELIBRARY_SHUTDOWN_SYMBOL_NAME);

	if ( !shutdownFunc )
	{
		return;
	}

	shutdownFunc();
}
