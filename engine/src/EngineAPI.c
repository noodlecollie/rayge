#include <stdbool.h>
#include "RayGE/Private/CurrentEngineAPI.h"
#include "Subsystems/LoggingSubsystem.h"

const RayGE_Engine_API_Current g_EngineAPI = {
	// Logging
	{
		LoggingSubsystem_PrintLine,
	}
};
