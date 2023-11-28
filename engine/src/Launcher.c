#include "RayGE/Private/Launcher.h"

RAYGE_ENGINE_PUBLIC(int32_t) RayGE_Launcher_Run(const RayGE_LaunchParams* params)
{
	if ( !RAYGE_INTERFACE_VERIFY(params, RAYGE_LAUNCHPARAMS_VERSION) )
	{
		return RAYGE_LAUNCH_FAILED_INVALID_PARAMS;
	}

	// TODO
	return 1;
}
