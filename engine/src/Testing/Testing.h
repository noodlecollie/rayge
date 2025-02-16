#pragma once

#include <stdint.h>
#include <stdbool.h>

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
bool Testing_ExpectTrue(bool result, const char* expression, const char* file, int line);
bool Testing_ExpectFalse(bool result, const char* expression, const char* file, int line);

bool Testing_ExpectIntegersEqual(
	int64_t lhs,
	int64_t rhs,
	bool checkEqual,
	const char* expression,
	const char* file,
	int line
);

bool Testing_ExpectFloatsExactlyEqual(
	double lhs,
	double rhs,
	bool checkEqual,
	const char* expression,
	const char* file,
	int line
);

bool Testing_ExpectFloatsApproxEqual(
	double lhs,
	double rhs,
	double tolerance,
	bool checkEqual,
	const char* expression,
	const char* file,
	int line
);

#define TEST_EXPECT_TRUE(expression) Testing_ExpectTrue((expression), #expression, __FILE__, __LINE__)
#define TEST_EXPECT_FALSE(expression) Testing_ExpectFalse((expression), #expression, __FILE__, __LINE__)

#define TEST_EXPECT_TRUE_CALLABLE(func, lhs, rhs) \
	Testing_ExpectTrue(func((lhs), (rhs)), #func ": " #lhs " == " #rhs, __FILE__, __LINE__)

#define TEST_EXPECT_FALSE_CALLABLE(func, lhs, rhs) \
	Testing_ExpectTrue(!func((lhs), (rhs)), "!" #func ": " #lhs " != " #rhs, __FILE__, __LINE__)

#define TEST_EXPECT_EQL_INT(lhs, rhs) \
	Testing_ExpectIntegersEqual((int64_t)(lhs), (int64_t)(rhs), true, #lhs " == " #rhs, __FILE__, __LINE__)

#define TEST_EXPECT_NEQL_INT(lhs, rhs) \
	Testing_ExpectIntegersEqual((int64_t)(lhs), (int64_t)(rhs), false, #lhs " != " #rhs, __FILE__, __LINE__)

#define TEST_EXPECT_EQL_FLOAT(lhs, rhs) \
	Testing_ExpectFloatsExactlyEqual((double)lhs, (double)rhs, true, #lhs " == " #rhs, __FILE__, __LINE__)

#define TEST_EXPECT_NEQL_FLOAT(lhs, rhs) \
	Testing_ExpectFloatsExactlyEqual((double)lhs, (double)rhs, false, #lhs " != " #rhs, __FILE__, __LINE__)

#define TEST_EXPECT_APRX_FLOAT(lhs, rhs, tolerance) \
	Testing_ExpectFloatsApproxEqual( \
		(double)lhs, \
		(double)rhs, \
		(double)tolerance, \
		true, \
		#lhs " - " #rhs " <= " #tolerance, \
		__FILE__, \
		__LINE__ \
	)

#define TEST_EXPECT_NAPRX_FLOAT(lhs, rhs, tolerance) \
	Testing_ExpectFloatsApproxEqual( \
		(double)lhs, \
		(double)rhs, \
		(double)tolerance, \
		false, \
		#lhs " - " #rhs " > " #tolerance, \
		__FILE__, \
		__LINE__ \
	)
#endif
