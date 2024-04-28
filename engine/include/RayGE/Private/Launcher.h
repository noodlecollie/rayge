// File: Launcher.h
// This file is private, and is not intended to be
// used by games.

#include <stdint.h>
#include <stddef.h>
#include "RayGE/Export.h"
#include "RayGE/Private/InterfaceUtils.h"

#define RAYGE_LAUNCHER_RUN_SYMBOL_NAME "RayGE_Launcher_Run"
#define RAYGE_LAUNCHPARAMS_VERSION 1

typedef enum RayGE_Launche_ExitCode
{
	RAYGE_LAUNCHER_EXIT_OK = 0,
	RAYGE_LAUNCHER_EXIT_UNKNOWN_ERROR,
	RAYGE_LAUNCHER_EXIT_LOG_FATAL_ERROR,
	RAYGE_LAUNCHER_EXIT_FAIL_ENGINE_LOAD,
	RAYGE_LAUNCHER_EXIT_FAIL_GAME_LOAD
} RayGE_Launcher_ExitCode;

typedef struct RayGE_LaunchParams
{
	RayGE_Interface_Header _header;

	size_t argc;
	const char* const* argv;

	RayGE_Interface_Footer _footer;
} RayGE_LaunchParams;

typedef int32_t(RAYGE_ENGINE_CDECL* RayGE_Launcher_Run_FuncPtr)(const RayGE_LaunchParams*);

// If return value is negative, it represents the negative value of the param
// struct version that was expected to be passed.
RAYGE_ENGINE_PUBLIC(int32_t) RayGE_Launcher_Run(const RayGE_LaunchParams* params);
