#include <stdio.h>
#include "RayGE/Private/Launcher.h"
#include "Platform/FileSystem.h"
#include "Platform/FileSystemNative.h"
#include "cJSON/cJSON.h"

#define DEFAULT_GAME_DIR "games/defaultgame"

bool TryLoadGameFromDirectory(const char* directory)
{
	Platform_Path path;

	sprintf_s(path, sizeof(path), "%s/game.json", directory);

	if ( !Platform_FileExists(path) )
	{
		// TODO: Log
		return false;
	}

	// TODO: Parse JSON to get library name
	// TODO: Load library by name
	// TODO: Pass interface to library
	return false;
}

RAYGE_ENGINE_PUBLIC(int32_t) RayGE_Launcher_Run(const RayGE_LaunchParams* params)
{
	if ( !RAYGE_INTERFACE_VERIFY(params, RAYGE_LAUNCHPARAMS_VERSION) || params->argc < 1 )
	{
		return RAYGE_LAUNCH_FAILED_INVALID_PARAMS;
	}

	Platform_SetExecutableFromArgV0(params->argv[0]);

	const char* gameDir = DEFAULT_GAME_DIR;

	if ( !Platform_DirectoryExists(gameDir) )
	{
		// TODO: Expect a command line argument to
		// specify the game to open
		return 1;
	}

	if ( !TryLoadGameFromDirectory(gameDir) )
	{
		// TODO: Better logging
		return 1;
	}

	// TODO
	return 1;
}
