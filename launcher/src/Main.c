#include <stdlib.h>
#include <stdio.h>
#include "RayGE/Private/Launcher.h"
#include "RayGE/Private/InterfaceUtils.h"
#include "RayGE/Platform.h"
#include "wzl_cutl/libloader.h"
#include "wzl_cutl/string.h"

#ifndef NDEBUG
// Debug
#define FIRST_ENGINE_PREFERENCE LIB_PREFIX LIBNAME_ENGINE_DEBUG LIB_EXTENSION
#define SECOND_ENGINE_PREFERENCE LIB_PREFIX LIBNAME_ENGINE LIB_EXTENSION
#define OPTION_SWAP_ENGINE_PREF "--prefer-release-engine"
#else
// Release
#define FIRST_ENGINE_PREFERENCE LIB_PREFIX LIBNAME_ENGINE LIB_EXTENSION
#define SECOND_ENGINE_PREFERENCE LIB_PREFIX LIBNAME_ENGINE_DEBUG LIB_EXTENSION
#define OPTION_SWAP_ENGINE_PREF "--prefer-debug-engine"
#endif

static const char* g_EngineLibPaths[2] =
{
	FIRST_ENGINE_PREFERENCE,
	SECOND_ENGINE_PREFERENCE,
};

static void SwapEngineLibPathPreference(void)
{
	static bool haveSwappedPrefs = false;

	if ( haveSwappedPrefs )
	{
		return;
	}

	const char* temp = g_EngineLibPaths[0];
	g_EngineLibPaths[0] = g_EngineLibPaths[1];
	g_EngineLibPaths[1] = temp;

	haveSwappedPrefs = true;
}

static void* LoadEngineLibrary(void)
{
	printf("Attempting to load engine library: %s\n", g_EngineLibPaths[0]);
	void* engineLibrary = wzl_load_library(g_EngineLibPaths[0]);

	if ( engineLibrary )
	{
		return engineLibrary;
	}

	printf("Failed to load %s, falling back to %s\n", g_EngineLibPaths[0], g_EngineLibPaths[1]);
	return wzl_load_library(g_EngineLibPaths[1]);
}

int main(int argc, char** argv)
{
	if ( argc < 1 )
	{
		return RAYGE_LAUNCHER_EXIT_UNKNOWN_ERROR;
	}

	for ( int index = 1; index < argc; ++index )
	{
		if ( wzl_strequals(argv[index], OPTION_SWAP_ENGINE_PREF) )
		{
			SwapEngineLibPathPreference();
		}
	}

	void* engineLibrary = LoadEngineLibrary();

	if ( !engineLibrary )
	{
		fprintf(stderr, "Failed to load engine library\n");
		return RAYGE_LAUNCHER_EXIT_FAIL_ENGINE_LOAD;
	}

	RayGE_Launcher_Run_FuncPtr runFuncPtr =
		(RayGE_Launcher_Run_FuncPtr)wzl_get_library_function(engineLibrary, RAYGE_LAUNCHER_RUN_SYMBOL_NAME);

	if ( !runFuncPtr )
	{
		fprintf(stderr, "Failed to look up function in engine library\n");
		return RAYGE_LAUNCHER_EXIT_FAIL_ENGINE_LOAD;
	}

	// Build a new param list, excluding any consumed params.
	// Apparently we need an extra null param on the end,
	// or cargs doesn't know where the end is...
	// (Surely it should use argc??)
	char** newParams = (char**)calloc(argc + 1, sizeof(char*));
	size_t newParamCount = 0;

	for ( int index = 0; index < argc; ++index )
	{
		if ( !wzl_strequals(argv[index], OPTION_SWAP_ENGINE_PREF) )
		{
			newParams[newParamCount++] = wzl_strdup(argv[index]);
		}
	}

	RayGE_LaunchParams params;
	RAYGE_INTERFACE_INIT(RayGE_LaunchParams, &params, RAYGE_LAUNCHPARAMS_VERSION);

	params.argc = newParamCount;
	params.argv = (const char* const*)newParams;

	const int32_t result = runFuncPtr(&params);

	// Free the new param list
	for ( size_t index = 0; index < newParamCount; ++index )
	{
		free(newParams[index]);
	}

	free(newParams);

	wzl_unload_library(engineLibrary);

	return result;
}
