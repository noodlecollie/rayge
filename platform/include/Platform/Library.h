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

typedef void (*Platform_OpaqueFuncPtr)(void);

void* Platform_LoadLibrary(const char* filename);
int32_t Platform_UnloadLibrary(void* handle);
const char* Platform_GetLastLibraryError(void);
void* Platform_LookUpLibrarySymbol(void* handle, const char* symbol);

// It is expected that the return of this function is cast to the proper function pointer type.
// This is only here to isolate the GCC diagnostic that we have to suppress (thanks, -pedantic).
static inline Platform_OpaqueFuncPtr Platform_LookUpLibraryFunction(void* handle, const char* symbol)
{
#ifndef _MSC_VER
// No way to do this without suppressing -pedantic...
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif // _MSC_VER

	return (Platform_OpaqueFuncPtr)Platform_LookUpLibrarySymbol(handle, symbol);

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif // _MSC_VER
}
