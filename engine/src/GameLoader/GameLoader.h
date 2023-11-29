#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "RayGE/Export.h"

typedef void*(
	RAYGE_ENGINE_CDECL* EngineLibFunc_GetEngineAPI)(uint64_t /* version */, uint64_t* /*outSupportedVersion*/);

typedef void(RAYGE_ENGINE_CDECL* GameLibFunc_Startup)(EngineLibFunc_GetEngineAPI /*getEngineApiPtr*/);
typedef void(RAYGE_ENGINE_CDECL* GameLibFunc_ShutDown)(void);

#define RAYGE_GAMELIBRARY_STARTUP_SYMBOL_NAME "RayGE_GameLibrary_Startup"
#define RAYGE_GAMELIBRARY_SHUTDOWN_SYMBOL_NAME "RayGE_GameLibrary_ShutDown"

bool GameLoader_InvokeGameLibraryStartup(void* gameLibrary);
void GameLoader_InvokeGameLibraryShutdown(void* gameLibrary);
