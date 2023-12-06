// File: Platform.h
// Provides definitions for supporting different platforms.

typedef enum RayGE_PlatformID
{
	RAYGE_PLATFORM_LINUX = 0,
	RAYGE_PLATFORM_WINDOWS
} RayGE_PlatformID;

#if defined(__linux__)
#define RAYGE_PLATFORM() RAYGE_PLATFORM_LINUX
#elif defined(_WIN32)
#define RAYGE_PLATFORM() RAYGE_PLATFORM_WINDOWS
#else
#error Unsupported platform
#endif

#if RAYGE_PLATFORM() == RAYGE_PLATFORM_LINUX
#define PLATFORM_LIB_PREFIX "lib"
#define PLATFORM_LIB_EXTENSION ".so"
#elif RAYGE_PLATFORM() == RAYGE_PLATFORM_WINDOWS
#define PLATFORM_LIB_PREFIX ""
#define PLATFORM_LIB_EXTENSION ".dll"
#endif
