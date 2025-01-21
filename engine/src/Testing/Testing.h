#pragma once

#ifdef RAYGE_BUILD_TESTING_FLAG
#define RAYGE_BUILD_TESTING() 1
#else
#define RAYGE_BUILD_TESTING() 0
#endif

#if RAYGE_BUILD_TESTING()

// Returns true if all succeeded, or false, if one or more tests failed.
bool Testing_RunAllTests(void);
void Testing_PrintResultsToLog(void);

// Only true when a test function is actually being called.
// Not true for 100% of the time Testing_RunAllTests() is running.
bool Testing_TestRunning(void);

// Returns the result of the expression.
bool Testing_RecordTestResult(bool result, const char* expression, const char* file, int line);

#define TESTING_CHECK(expression) Testing_RecordTestResult((expression), #expression, __FILE__, __LINE__)
#endif
