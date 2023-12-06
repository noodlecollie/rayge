#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "RayGE/Engine.h"

typedef void(RAYGE_ENGINE_CDECL* GameLibFunc_Startup)(RayGE_Engine_GetAPIFunc /*getEngineApiPtr*/);
typedef void(RAYGE_ENGINE_CDECL* GameLibFunc_ShutDown)(void);

#define RAYGE_GAMELIBRARY_STARTUP_SYMBOL_NAME "RayGE_GameLibrary_Startup"
#define RAYGE_GAMELIBRARY_SHUTDOWN_SYMBOL_NAME "RayGE_GameLibrary_ShutDown"

bool GameLoader_InvokeGameLibraryStartup(void* gameLibrary);
void GameLoader_InvokeGameLibraryShutdown(void* gameLibrary);
