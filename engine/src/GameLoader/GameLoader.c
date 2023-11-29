#include <stddef.h>
#include "GameLoader/GameLoader.h"
#include "Platform/Library.h"

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
		(GameLibFunc_Startup)Platform_LookUpLibraryFunction(gameLibrary, RAYGE_GAMELIBRARY_STARTUP_SYMBOL_NAME);

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
		(GameLibFunc_ShutDown)Platform_LookUpLibraryFunction(gameLibrary, RAYGE_GAMELIBRARY_SHUTDOWN_SYMBOL_NAME);

	if ( !shutdownFunc )
	{
		return;
	}

	shutdownFunc();
}
