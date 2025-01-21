#pragma once

#include <stdbool.h>
#include "RayGE/Private/Launcher.h"
#include "Logging/Logging.h"

typedef struct RayGE_LaunchState
{
	RayGE_Log_Level defaultLogLevel;
	bool enableBackendDebugLogs;
	bool enableMemPoolDebugging;
	bool runTestsAndExit;
} RayGE_LaunchState;

bool LaunchParams_Parse(const RayGE_LaunchParams* params);
const RayGE_LaunchState* LaunchParams_GetLaunchState(void);
