#include <stdlib.h>
#include "RayGE/Private/Launcher.h"
#include "RayGE/InterfaceUtils.h"
#include "Platform/Library.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	if ( argc < 1 )
	{
		return -1;
	}

	void* engineLibrary = Platform_LoadLibrary(PLATFORM_LIB_PREFIX LIBNAME_ENGINE PLATFORM_LIB_EXTENSION);

	if ( !engineLibrary )
	{
		// TODO: More comprehensive function calls
		printf("Failed to load engine library\n");
		return -1;
	}

	RayGE_Launcher_Run_FuncPtr runFuncPtr =
		(RayGE_Launcher_Run_FuncPtr)Platform_LookUpLibraryFunction(engineLibrary, RAYGE_LAUNCHER_RUN_SYMBOL_NAME);

	if ( !runFuncPtr )
	{
		// TODO: More comprehensive function calls
		printf("Failed to look up function in engine library\n");
		return -1;
	}

	RayGE_LaunchParams params;
	RAYGE_INTERFACE_INIT(RayGE_LaunchParams, &params, RAYGE_LAUNCHPARAMS_VERSION);

	params.argc = (size_t)argc;
	params.argv = (const char* const*)argv;

	const int32_t result = runFuncPtr(&params);

	Platform_UnloadLibrary(engineLibrary);

	return result;
}
