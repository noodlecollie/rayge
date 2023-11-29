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

// No way to do this without suppressing -pedantic...
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	GameLibFunc_Startup startupFunc =
		(GameLibFunc_Startup)Platform_LookUpLibrarySymbol(gameLibrary, GAMELIBSYMBOL_STARTUP);
#pragma GCC diagnostic pop

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

// No way to do this without suppressing -pedantic...
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	GameLibFunc_ShutDown shutdownFunc =
		(GameLibFunc_ShutDown)Platform_LookUpLibrarySymbol(gameLibrary, GAMELIBSYMBOL_SHUTDOWN);
#pragma GCC diagnostic pop

	if ( !shutdownFunc )
	{
		return;
	}

	shutdownFunc();
}
