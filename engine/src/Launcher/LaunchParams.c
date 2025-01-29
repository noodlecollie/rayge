#include <stdlib.h>
#include "Launcher/LaunchParams.h"
#include "Logging/Logging.h"
#include "MemPool/MemPoolManager.h"
#include "Identity/Identity.h"
#include "Testing/Testing.h"
#include "Debugging.h"
#include "cargs.h"

#if RAYGE_DEBUG()
#define MEMPOOL_DEBUG_DEFAULT true
#define MEMPOOL_DEBUG_DEFAULT_STR "true"
#else
#define MEMPOOL_DEBUG_DEFAULT false
#define MEMPOOL_DEBUG_DEFAULT_STR "false"
#endif

typedef enum OptionIdentifier
{
	ID_HELP = (int)'A',
	ID_VERSION,
	ID_DEBUG_MEMPOOL,
	ID_RUN_TESTS,
	ID_VERBOSE_TESTS,
	ID_DEV_LEVEL,
} OptionIdentifier;

static RayGE_LaunchState g_LaunchState;
static const struct cag_option LaunchOptionDefs[] = {
	{
		.identifier = (char)ID_HELP,
		.access_letters = "h",
		.access_name = "help",
		.description = "Displays a help message and exits.",
	},
	{
		.identifier = (char)ID_VERSION,
		.access_letters = "v",
		.access_name = "version",
		.description = "Displays version string and exits.",
	},
	{
		.identifier = (char)ID_DEBUG_MEMPOOL,
		.access_letters = NULL,
		.access_name = "debug-mempool",
		.description =
			"Enables debugging of memory pool allocations (default for this build: " MEMPOOL_DEBUG_DEFAULT_STR
			"). This may affect performance.",
	},
#if RAYGE_BUILD_TESTING()
	{
		.identifier = (char)ID_RUN_TESTS,
		.access_letters = "t",
		.access_name = "run-tests",
		.description = "If set, runs tests on launch, prints their results, and exits.",
	},
	{
		.identifier = (char)ID_VERBOSE_TESTS,
		.access_letters = NULL,
		.access_name = "verbose-tests",
		.description = "If set alongside --run-tests, results of the tests will be logged verbosely.",
	},
#endif
	{
		.identifier = (char)ID_DEV_LEVEL,
		.access_letters = NULL,
		.access_name = "dev",
		.value_name = "DEV_LEVEL",
		.description =
			"Sets the developer level (defaults to 0). Higher levels make more debugging features available.",
	},
};

static void SetDefaults(RayGE_LaunchState* state)
{
	state->defaultLogLevel = RAYGE_LOG_INFO;
	state->enableBackendDebugLogs = false;
	state->enableMemPoolDebugging = MEMPOOL_DEBUG_DEFAULT;
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
			case ID_HELP:
			{
				cag_option_print(LaunchOptionDefs, CAG_ARRAY_SIZE(LaunchOptionDefs), stdout);

				// Quit here.
				return false;
			}

			case ID_VERSION:
			{
				printf("RayGE %s\n", Identity_GetBuildDescription());

				// Quit here.
				return false;
			}

			case ID_DEV_LEVEL:
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

			case ID_DEBUG_MEMPOOL:
			{
				g_LaunchState.enableMemPoolDebugging = true;
				break;
			}

#if RAYGE_BUILD_TESTING()
			case ID_RUN_TESTS:
			{
				g_LaunchState.runTestsAndExit = true;
				break;
			}

			case ID_VERBOSE_TESTS:
			{
				g_LaunchState.runTestsVerbose = true;
				break;
			}
#endif

			case '?':
			default:
			{
				cag_option_print_error(&context, stderr);

				const int index = cag_option_get_error_index(&context);

				// If someone's passed -abc instead of --abc, print a hint about it.
				if ( index >= 0 && (size_t)index < params->argc && params->argv[index][0] == '-' &&
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
