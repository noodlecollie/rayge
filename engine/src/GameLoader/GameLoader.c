#include <stddef.h>
#include "GameLoader/GameLoader.h"
#include "wzl_cutl/libloader.h"

static void* GetEngineAPI(uint64_t version, uint64_t* outSupportedVersion)
{
	// TODO: Implement
	(void)version;

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
