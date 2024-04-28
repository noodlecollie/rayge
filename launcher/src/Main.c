#include <stdlib.h>
#include "RayGE/Private/Launcher.h"
#include "RayGE/Private/InterfaceUtils.h"
#include "RayGE/Platform.h"
#include "wzl_cutl/libloader.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	if ( argc < 1 )
	{
		return RAYGE_LAUNCHER_EXIT_UNKNOWN_ERROR;
	}

	void* engineLibrary = wzl_load_library(LIB_PREFIX LIBNAME_ENGINE LIB_EXTENSION);

	if ( !engineLibrary )
	{
		// TODO: More comprehensive function calls
		fprintf(stderr, "Failed to load engine library\n");
		return RAYGE_LAUNCHER_EXIT_FAIL_ENGINE_LOAD;
	}

	RayGE_Launcher_Run_FuncPtr runFuncPtr =
		(RayGE_Launcher_Run_FuncPtr)wzl_get_library_function(engineLibrary, RAYGE_LAUNCHER_RUN_SYMBOL_NAME);

	if ( !runFuncPtr )
	{
		// TODO: More comprehensive function calls
		fprintf(stderr, "Failed to look up function in engine library\n");
		return RAYGE_LAUNCHER_EXIT_FAIL_ENGINE_LOAD;
	}

	RayGE_LaunchParams params;
	RAYGE_INTERFACE_INIT(RayGE_LaunchParams, &params, RAYGE_LAUNCHPARAMS_VERSION);

	params.argc = (size_t)argc;
	params.argv = (const char* const*)argv;

	const int32_t result = runFuncPtr(&params);

	wzl_unload_library(engineLibrary);

	return result;
}
