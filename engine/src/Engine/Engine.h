#pragma once

#include <stdint.h>
#include "Testing/Testing.h"

void Engine_StartUp(void);
void Engine_ShutDown(void);
void Engine_RunToCompletion(void);

#if RAYGE_BUILD_TESTING()
// Starts up minimally, runs tests, and shuts down.
int32_t Engine_RunTestsOnly(void);
#endif
