// File: Launcher.h
// This file is private, and is not intended to be
// used by games.

#include <stdint.h>
#include <stddef.h>
#include "RayGE/Export.gen"
#include "RayGE/InterfaceUtils.h"

#define RAYGE_LAUNCH_FAILED_INVALID_PARAMS -1000

#define RAYGE_LAUNCHPARAMS_VERSION 1

typedef struct RayGE_LaunchParams
{
	RayGE_Interface_Header _header;

	size_t argc;
	const char* const* argv;

	RayGE_Interface_Footer _footer;
} RayGE_LaunchParams;

RAYGE_ENGINE_PUBLIC(int32_t) RayGE_Launcher_Run(const RayGE_LaunchParams* params);
