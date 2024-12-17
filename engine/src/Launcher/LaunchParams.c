#include <stdlib.h>
#include "Launcher/LaunchParams.h"
#include "Logging/Logging.h"
#include "MemPool/MemPoolManager.h"
#include "Identity/Identity.h"
#include "Debugging.h"
#include "cargs.h"

static RayGE_LaunchState g_LaunchState;
static const struct cag_option LaunchOptionDefs[] = {
	{
		.identifier = 'h',
		.access_letters = "h",
		.access_name = "help",
		.description = "Displays a help message and exits.",
	},
	{
		.identifier = 'v',
		.access_letters = "v",
		.access_name = "version",
		.description = "Displays version string and exits.",
	},
	{
		.identifier = 'd',
		.access_letters = NULL,
		.access_name = "dev",
		.value_name = "DEV_LEVEL",
		.description =
			"Sets the developer level (defaults to 0). Higher levels make more debugging features available.",
	},
	{
		.identifier = 'm',
		.access_letters = NULL,
		.access_name = "debug-mempool",
		.description = "Enables debugging of memory pool allocations. This may affect performance",
	}
};

static void SetDefaults(RayGE_LaunchState* state)
{
	state->defaultLogLevel = RAYGE_LOG_INFO;
	state->enableBackendDebugLogs = false;

#if RAYGE_DEBUG()
	state->enableMemPoolDebugging = true;
#else
	state->enableMemPoolDebugging = false;
#endif
}

bool LaunchParams_Parse(const RayGE_LaunchParams* params)
{
	SetDefaults(&g_LaunchState);

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

			case 'v':
			{
				printf("RayGE %s\n", Identity_GetBuildDescription());

				// Quit here.
				return false;
			}

			case 'd':
			{
				const char* value = cag_option_get_value(&context);
				int level = value ? atoi(value) : 0;

				if ( level >= 3 )
				{
					g_LaunchState.enableBackendDebugLogs = true;
					g_LaunchState.defaultLogLevel = RAYGE_LOG_TRACE;
				}
				else if ( level == 2 )
				{
					g_LaunchState.enableBackendDebugLogs = false;
					g_LaunchState.defaultLogLevel = RAYGE_LOG_TRACE;
				}
				else if ( level == 1 )
				{
					g_LaunchState.enableBackendDebugLogs = false;
					g_LaunchState.defaultLogLevel = RAYGE_LOG_DEBUG;
				}
				else
				{
					g_LaunchState.enableBackendDebugLogs = false;
					g_LaunchState.defaultLogLevel = RAYGE_LOG_INFO;
				}

				break;
			}

			case 'm':
			{
				g_LaunchState.enableMemPoolDebugging = true;
				break;
			}

			case '?':
			default:
			{
				cag_option_print_error(&context, stderr);

				const int index = cag_option_get_error_index(&context);

				// If someone's passed -abc instead of --abc, print a hint about it.
				if ( index >= 0 && index < params->argc && params->argv[index][0] == '-' &&
					 strlen(params->argv[index]) > 2 )
				{
					fprintf(stderr, "This looks like a long option - did you mean -%s?\n", params->argv[index]);
				}

				// Quit here.
				return false;
			}
		}
	}

	return true;
}

const RayGE_LaunchState* LaunchParams_GetLaunchState(void)
{
	return &g_LaunchState;
}
