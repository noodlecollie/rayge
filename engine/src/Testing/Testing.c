#include <stdbool.h>
#include <math.h>
#include "Testing/Testing.h"
#include "MemPool/MemPoolManager.h"
#include "Resources/ResourceList.h"
#include "Testing/AngleTests.h"
#include "Launcher/LaunchParams.h"
#include "Debugging.h"

#if !RAYGE_BUILD_TESTING()
#error Testing is not enabled, so this file should not be included in the build!
#endif

#define RESULT_LIST_INCREMENT 16
#define TEST_CATEGORY_NAME_LENGTH 32

typedef struct TestCheckResult
{
	char category[TEST_CATEGORY_NAME_LENGTH];
	bool passed;
	const char* expression;
	const char* file;
	int line;
} TestCheckResult;

typedef struct Data
{
	TestCheckResult* testResultsList;
	size_t testResultsListCapacity;
	size_t numTestResults;
	size_t numTestsSuccessful;
	size_t numTestsFailed;

	bool testRunning;
	char currentTestCategory[TEST_CATEGORY_NAME_LENGTH];
} Data;

static Data g_Data;

static void EnsureNewSpaceInResultList(void)
{
	if ( !g_Data.testResultsList )
	{
		g_Data.numTestResults = 0;
		g_Data.testResultsListCapacity = RESULT_LIST_INCREMENT;
		g_Data.testResultsList =
			MEMPOOL_MALLOC(MEMPOOL_TEST_MANAGER, g_Data.testResultsListCapacity * sizeof(TestCheckResult));
	}

	if ( g_Data.numTestResults >= g_Data.testResultsListCapacity )
	{
		g_Data.testResultsListCapacity += RESULT_LIST_INCREMENT;

		g_Data.testResultsList = MEMPOOL_REALLOC(
			MEMPOOL_TEST_MANAGER,
			g_Data.testResultsList,
			g_Data.testResultsListCapacity * sizeof(TestCheckResult)
		);
	}
}

static void RunTestsInCategory(const char* category, void (*func)(void))
{
	wzl_strcpy(g_Data.currentTestCategory, sizeof(g_Data.currentTestCategory), category);

	Logging_PrintLine(RAYGE_LOG_INFO, "** START tests for [%s] **", g_Data.currentTestCategory);

	g_Data.testRunning = true;
	func();
	g_Data.testRunning = false;

	Logging_PrintLine(RAYGE_LOG_INFO, "**  END  tests for [%s] **", g_Data.currentTestCategory);

	g_Data.currentTestCategory[0] = '\0';
}

bool RecordTestResult(bool passed, const char* expression, const char* file, int line)
{
	if ( !g_Data.testRunning )
	{
		// This should not be called if we're not running tests!
		// Don't log because we don't know the state of that system,
		// but do break in the debugger.
		RayGE_DebugBreak();
		return passed;
	}

	if ( LaunchParams_GetLaunchState()->runTestsVerbose )
	{
		Logging_PrintLine(RAYGE_LOG_INFO, "[%s] %s:%d: %s", passed ? "PASS" : "FAIL", file, line, expression);
	}

	EnsureNewSpaceInResultList();

	TestCheckResult* item = &g_Data.testResultsList[g_Data.numTestResults];

	wzl_strcpy(item->category, sizeof(item->category), g_Data.currentTestCategory);

	item->passed = passed;
	item->expression = expression;
	item->file = file;
	item->line = line;

	++g_Data.numTestResults;

	if ( item->passed )
	{
		++g_Data.numTestsSuccessful;
	}
	else
	{
		++g_Data.numTestsFailed;
	}

	return passed;
}

bool Testing_RunAllTests(void)
{
	if ( g_Data.testResultsList )
	{
		MEMPOOL_FREE(g_Data.testResultsList);
		g_Data.testResultsListCapacity = 0;
		g_Data.numTestResults = 0;
		g_Data.numTestsSuccessful = 0;
		g_Data.numTestsFailed = 0;
	}

	RunTestsInCategory("MemPool Realloc", &MemPoolManager_TestRealloc);
	RunTestsInCategory("Resource List", &ResourceList_RunTests);
	RunTestsInCategory("Angle Normalisation", &Testing_RunAngleNormalisationTests);

	if ( g_Data.numTestsFailed > 0 )
	{
		Logging_PrintLine(RAYGE_LOG_ERROR, "%zu of %zu tests failed!", g_Data.numTestsFailed, g_Data.numTestResults);
	}

	return g_Data.numTestsFailed < 1;
}

void Testing_PrintResultsToLog(void)
{
	Logging_PrintLine(RAYGE_LOG_INFO, "\n===== TEST RESULTS =====");

	Logging_PrintLine(
		RAYGE_LOG_INFO,
		"  %zu tests successful, %zu failed, of %zu total (%.2f%% passed)",
		g_Data.numTestsSuccessful,
		g_Data.numTestsFailed,
		g_Data.numTestResults,
		g_Data.numTestResults != 0 ? (((float)g_Data.numTestsSuccessful / (float)g_Data.numTestResults) * 100.0f) : 0.0f
	);

	if ( g_Data.numTestsFailed < 1 )
	{
		return;
	}

	Logging_PrintLine(RAYGE_LOG_INFO, "Failed tests:");

	for ( size_t index = 0; index < g_Data.numTestResults; ++index )
	{
		TestCheckResult* item = &g_Data.testResultsList[index];

		if ( item->passed )
		{
			continue;
		}

		Logging_PrintLine(
			RAYGE_LOG_INFO,
			"  [%s] %s:%d: \"%s\"",
			item->category,
			item->file,
			item->line,
			item->expression
		);
	}

	Logging_PrintLineStr(RAYGE_LOG_INFO, "");
}

bool Testing_TestRunning(void)
{
	return g_Data.testRunning;
}

bool Testing_ExpectTrue(bool result, const char* expression, const char* file, int line)
{
	return RecordTestResult(result, expression, file, line);
}

bool Testing_RecordTestResult_ExpectFalse(bool result, const char* expression, const char* file, int line)
{
	return RecordTestResult(!result, expression, file, line);
}

bool Testing_ExpectIntegersEqual(
	int64_t lhs,
	int64_t rhs,
	bool checkEqual,
	const char* expression,
	const char* file,
	int line
)
{
	return RecordTestResult((lhs == rhs) == checkEqual, expression, file, line);
}

bool Testing_ExpectFloatsExactlyEqual(
	double lhs,
	double rhs,
	bool checkEqual,
	const char* expression,
	const char* file,
	int line
)
{
	return RecordTestResult((lhs == rhs) == checkEqual, expression, file, line);
}

bool Testing_ExpectFloatsApproxEqual(
	double lhs,
	double rhs,
	double tolerance,
	bool checkEqual,
	const char* expression,
	const char* file,
	int line
)
{
	return RecordTestResult((fabs(lhs - rhs) <= tolerance) == checkEqual, expression, file, line);
}
