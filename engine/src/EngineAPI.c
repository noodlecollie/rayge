#include <stdbool.h>
#include "RayGE/Engine.h"
#include "LoggingSubsystem/LoggingSubsystem.h"

const RayGE_Engine_API g_EngineAPI = {
	// Logging
	{
		LoggingSubsystem_EmitMessage,
	}
};
