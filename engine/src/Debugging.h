#pragma once

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include "Logging/Logging.h"
#include "Testing/Testing.h"
#include "wzl_cutl/string.h"

#ifndef NDEBUG
#define RAYGE_DEBUG() 1
#else
#define RAYGE_DEBUG() 0
#endif

typedef enum InvariantFailureType
{
	// Non-fatal, prints an error.
	INVARIANT_FAILURE_EXPECT,

	// Fatal, but may be side-stepped eg. for
	// running tests that trigger known failure conditions.
	// Should only be triggered in debug builds.
	INVARIANT_FAILURE_ASSERT,

	// Always fatal.
	INVARIANT_FAILURE_ENSURE,
} InvariantFailureType;

void RayGE_DebugBreak(void);

static inline void RayGE_CheckInvariant(
	InvariantFailureType type,
	bool expression,
	const char* expressionStr,
	const char* file,
	int line,
	const char* function,
	const char* description,
	...
)
{
	if ( expression )
	{
		return;
	}

#if RAYGE_BUILD_TESTING()
	const bool shouldActOnAssertionFailure = !Testing_TestRunning();
#else
	const bool shouldActOnAssertionFailure = true;
#endif

	char descBuffer[LOG_MESSAGE_MAX_LENGTH];
	descBuffer[0] = '\0';

	if ( description )
	{
		va_list args;
		va_start(args, description);
		wzl_vsprintf(descBuffer, sizeof(descBuffer), description, args);
		va_end(args);
	}

	if ( type == INVARIANT_FAILURE_EXPECT )
	{
		// Since these may be used to trace error conditions in release builds,
		// don't make them too verbose.
		if ( descBuffer[0] )
		{
			Logging_PrintLine(RAYGE_LOG_ERROR, "%s:%d: %s", file ? file : "unknown-file", line, descBuffer);
		}
		else
		{
			Logging_PrintLine(
				RAYGE_LOG_ERROR,
				"%s:%d: Invariant \"%s\" was violated",
				file ? file : "unknown-file",
				line,
				expressionStr
			);
		}
	}
	else
	{
		if ( type != INVARIANT_FAILURE_ASSERT || shouldActOnAssertionFailure )
		{
			Logging_PrintLine(
				RAYGE_LOG_FATAL,
				"\n"
				"**** %s ****\n"
				"  File: %s:%d\n"
				"  Function: %s\n"
				"  Expression: %s"
				"%s%s",
				type == INVARIANT_FAILURE_ASSERT ? "ASSERTION FAILED" : "FATAL ERROR",
				file ? file : "unknown-file",
				line,
				function ? function : "unknown-function",
				expressionStr ? expressionStr : "unknown-expression",
				descBuffer[0] ? "\n  " : "",
				descBuffer[0] ? descBuffer : ""
			);
		}
	}
}

#define RAYGE_CHECK_INVARIANT(type, expr, ...) \
	do \
	{ \
		const bool exprResult = !!(expr); \
		if ( !exprResult ) \
		{ \
			RayGE_CheckInvariant((type), exprResult, (#expr), __FILE__, __LINE__, __func__, __VA_ARGS__); \
		} \
	} \
	while ( false )

// ENSURE macros are fatal if the condition fails.
#define RAYGE_ENSURE(expr, ...) RAYGE_CHECK_INVARIANT(INVARIANT_FAILURE_ENSURE, expr, __VA_ARGS__)
#define RAYGE_ENSURE_VALID(expr) RAYGE_CHECK_INVARIANT(INVARIANT_FAILURE_ENSURE, expr, "Required state was not valid")

// EXPECT macros are non-fatal and print an error.
#define RAYGE_EXPECT(expr, ...) RAYGE_CHECK_INVARIANT(INVARIANT_FAILURE_EXPECT, expr, __VA_ARGS__)

// FATAL macros always halt execution if they are hit.
#define RAYGE_FATAL_EX(conditionStr, ...) \
	do \
	{ \
		RayGE_CheckInvariant( \
			INVARIANT_FAILURE_ENSURE, \
			false, \
			conditionStr, \
			__FILE__, \
			__LINE__, \
			__func__, \
			__VA_ARGS__ \
		); \
	} \
	while ( false )

#define RAYGE_FATAL(...) RAYGE_FATAL_EX("<Fatal Condition>", __VA_ARGS__)

// Only active in debug builds:
#if RAYGE_DEBUG()

// ASSERTS in debug, does nothing in release
#define RAYGE_ASSERT(expr, ...) RAYGE_CHECK_INVARIANT(INVARIANT_FAILURE_ASSERT, expr, __VA_ARGS__)
#define RAYGE_ASSERT_VALID(expr) RAYGE_CHECK_INVARIANT(INVARIANT_FAILURE_ASSERT, expr, "Required state was not valid")
#define RAYGE_ASSERT_UNREACHABLE(...) RAYGE_FATAL_EX("<Encountered Unreachable Code>", __VA_ARGS__)

// ASSERTS in debug, EXPECTS in release
#define RAYGE_ASSERT_EXPECT(expr, ...) RAYGE_ASSERT(expr, __VA_ARGS__)

// Breaks in debug but does not log (useful if logging would be dangerous in the circumstances)
#define RAYGE_ASSERT_BREAK(expr) \
	do \
	{ \
		if ( !(expr) ) \
		{ \
			RayGE_DebugBreak(); \
		} \
	} \
	while ( false )
#else
#define RAYGE_ASSERT(expr, ...)
#define RAYGE_ASSERT_VALID(expr)
#define RAYGE_ASSERT_UNREACHABLE(...)
#define RAYGE_ASSERT_EXPECT(expr, ...) RAYGE_EXPECT(expr, __VA_ARGS__)
#define RAYGE_ASSERT_BREAK(expr)
#endif
