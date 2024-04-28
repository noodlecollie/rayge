// File: Platform.h
// Provides definitions for supporting different platforms.

#pragma once

#define RAYGE_PLATFORM_LINUX 0
#define RAYGE_PLATFORM_WINDOWS 1

#if defined(__linux__)
#define RAYGE_PLATFORM() RAYGE_PLATFORM_LINUX
#elif defined(_WIN32)
#define RAYGE_PLATFORM() RAYGE_PLATFORM_WINDOWS
#else
#error Unsupported platform
#endif
