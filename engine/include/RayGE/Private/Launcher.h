// File: Launcher.h
// This file is private, and is not intended to be
// used by games.

#include <stdint.h>
#include <stddef.h>
#include "RayGE/Export.h"
#include "RayGE/InterfaceUtils.h"

#define RAYGE_LAUNCHER_RUN_SYMBOL_NAME "RayGE_Launcher_Run"
#define RAYGE_LAUNCHPARAMS_VERSION 1

typedef struct RayGE_LaunchParams
{
	RayGE_Interface_Header _header;

	size_t argc;
	const char* const* argv;

	RayGE_Interface_Footer _footer;
} RayGE_LaunchParams;

typedef int32_t(RAYGE_ENGINE_CDECL* RayGE_Launcher_Run_FuncPtr)(const RayGE_LaunchParams*);

RAYGE_ENGINE_PUBLIC(int32_t) RayGE_Launcher_Run(const RayGE_LaunchParams* params);
