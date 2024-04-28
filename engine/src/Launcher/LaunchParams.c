#include <stdlib.h>
#include "Launcher/LaunchParams.h"
#include "Subsystems/LoggingSubsystem.h"
#include "cargs.h"

static const struct cag_option LaunchOptionDefs[] = {
	{
		.identifier = 'h',
		.access_letters = "h",
		.access_name = "help",
		.description = "Displays a help message and exits.",
	},
	{
		.identifier = 'd',
		.access_letters = NULL,
		.access_name = "dev",
		.value_name = "DEV_LEVEL",
		.description = "Sets the developer level (defaults to 0). Higher levels make more debugging features available.",
	}
};

bool LaunchParams_Parse(const RayGE_LaunchParams* params)
{
	if ( !params )
	{
		// Nothing to do.
		return true;
	}

	cag_option_context context;

	cag_option_prepare(
		&context,
		LaunchOptionDefs,
		CAG_ARRAY_SIZE(LaunchOptionDefs),
		(int)params->argc,
		(char**)params->argv
	);

	while ( cag_option_fetch(&context) )
	{
		switch ( cag_option_get(&context) )
		{
			case 'h':
			{
				cag_option_print(LaunchOptionDefs, CAG_ARRAY_SIZE(LaunchOptionDefs), stdout);

				// Quit here.
				return false;
			}

			case 'd':
			{
				const char* value = cag_option_get_value(&context);
				int level = value ? atoi(value) : 0;

				// TODO: Move this to some static state manager.
				if ( level >= 3 )
				{
					LoggingSubsystem_SetBackendDebugLogsEnabled(true);
					LoggingSubsystem_SetLogLevel(RAYGE_LOG_TRACE);
				}
				else if ( level == 2 )
				{
					LoggingSubsystem_SetBackendDebugLogsEnabled(false);
					LoggingSubsystem_SetLogLevel(RAYGE_LOG_TRACE);
				}
				else if ( level == 1 )
				{
					LoggingSubsystem_SetBackendDebugLogsEnabled(false);
					LoggingSubsystem_SetLogLevel(RAYGE_LOG_DEBUG);
				}
				else
				{
					LoggingSubsystem_SetBackendDebugLogsEnabled(false);
					LoggingSubsystem_SetLogLevel(RAYGE_LOG_INFO);
				}

				break;
			}
		}
	}

	return true;
}
