#include "GameLoader/GameLoader.h"
#include <dlfcn.h>
#include <stddef.h>

static void* GetEngineAPI(uint64_t version, uint64_t* outSupportedVersion)
{
	// TODO: Implement

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

	GameLibFunc_Startup startupFunc = (GameLibFunc_Startup)dlsym(gameLibrary, GAMELIBSYMBOL_STARTUP);

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
		return false;
	}

	GameLibFunc_ShutDown shutdownFunc = (GameLibFunc_ShutDown)dlsym(gameLibrary, GAMELIBSYMBOL_SHUTDOWN);

	if ( !shutdownFunc )
	{
		return false;
	}

	shutdownFunc();
	return true;
}
