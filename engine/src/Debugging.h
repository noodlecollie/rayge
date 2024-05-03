#pragma once

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include "Subsystems/LoggingSubsystem.h"
#include "wzl_cutl/string.h"

#ifndef NDEBUG
#define RAYGE_DEBUG() 1
#else
#define RAYGE_DEBUG() 0
#endif

static inline void RayGE_EnsureTrue(
	bool isFatal,
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

	char descBuffer[256];
	descBuffer[0] = '\0';

	if ( description )
	{
		va_list args;
		va_start(args, description);
		wzl_vsprintf(descBuffer, sizeof(descBuffer), description, args);
		va_end(args);
	}

	LoggingSubsystem_PrintLine(
		isFatal ? RAYGE_LOG_FATAL : RAYGE_LOG_WARNING,
		"ASSERTION FAILED: %s:%d (%s): %s%s%s",
		file ? file : "unknown-file",
		line,
		function ? function : "unknown-function",
		expressionStr ? expressionStr : "unknown-expression",
		descBuffer[0] ? " - " : "",
		descBuffer[0] ? descBuffer : ""
	);
}

#define RAYGE_ENSURE_EX(isFatal, expr, ...) \
	do \
	{ \
		bool exprResult = !!(expr); \
		if ( !exprResult ) \
		{ \
			RayGE_EnsureTrue((isFatal), exprResult, (#expr), __FILE__, __LINE__, __func__, __VA_ARGS__); \
		} \
	} \
	while ( false )

#define RAYGE_ENSURE(expr, ...) RAYGE_ENSURE_EX(true, expr, __VA_ARGS__)
#define RAYGE_EXPECT(expr, ...) RAYGE_ENSURE_EX(false, expr, __VA_ARGS__)

// Only active in debug builds:
#if RAYGE_DEBUG()
#define RAYGE_ASSERT(expr, ...) RAYGE_ENSURE(expr, __VA_ARGS__)
#else
#define RAYGE_ASSERT(expr, ...)
#endif
