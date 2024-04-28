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

	char descBuffer[128];
	descBuffer[0] = '\0';

	if ( description )
	{
		va_list args;
		va_start(args, description);
		wzl_vsprintf(descBuffer, sizeof(descBuffer), description, args);
		va_end(args);
	}

	LoggingSubsystem_PrintLine(
		RAYGE_LOG_FATAL,
		"ASSERTION FAILED: %s:%d (%s): %s%s%s",
		file ? file : "unknown-file",
		line,
		function ? function : "unknown-function",
		expressionStr ? expressionStr : "unknown-expression",
		descBuffer[0] ? " - " : "",
		descBuffer[0] ? descBuffer : ""
	);
}

#if RAYGE_DEBUG()
#define RAYGE_ASSERT(expr, ...) RayGE_EnsureTrue((expr), (#expr), __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define RAYGE_ASSERT(expr, ...)
#endif
