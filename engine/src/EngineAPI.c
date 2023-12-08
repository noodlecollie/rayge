#include <stdbool.h>
#include "RayGE/Engine.h"
#include "Subsystems/LoggingSubsystem.h"

const RayGE_Engine_API g_EngineAPI = {
	// Logging
	{
		LoggingSubsystem_PrintLine,
	}
};
