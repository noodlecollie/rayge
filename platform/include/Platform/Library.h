#pragma once

#include <stdint.h>

#if defined(__linux__)
#define PLATFORM_LIB_PREFIX "lib"
#define PLATFORM_LIB_EXTENSION ".so"
#elif defined(_WIN32)
#define PLATFORM_LIB_PREFIX ""
#define PLATFORM_LIB_EXTENSION ".dll"
#else
#error Unsupported platform
#endif
