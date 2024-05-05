#pragma once

#ifdef RAYGE_BUILD_TESTING_FLAG
#define RAYGE_BUILD_TESTING() 1
#else
#define RAYGE_BUILD_TESTING() 0
#endif
