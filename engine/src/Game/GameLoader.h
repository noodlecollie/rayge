#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "RayGE/APIs/Engine.h"
#include "wzl_cutl/attributes.h"

typedef void(RAYGE_ENGINE_CDECL* GameLibFunc_ExchangeAPIs)(RayGE_Engine_GetAPIFunc /*getEngineApiPtr*/);

#define RAYGE_GAMELIBRARY_EXCHANGEAPIS_SYMBOL_NAME "RayGE_GameLibrary_ExchangeAPIs"

WZL_ATTR_NODISCARD void* GameLoader_LoadLibraryFromDirectory(const char* dirPath);

// Does not call shutdown - assumes this has already been done.
void GameLoader_UnloadLibrary(void* handle);
