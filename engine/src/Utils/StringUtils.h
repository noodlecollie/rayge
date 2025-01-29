#pragma once

#include "MemPool/MemPoolManager.h"
#include "wzl_cutl/attributes.h"

typedef struct StringBounds
{
	const char* begin;
	const char* end;
} StringBounds;

StringBounds StringUtils_GetStringTrimBounds(const char* str);

// Caller is responsible for freeing
WZL_ATTR_NODISCARD char* StringUtils_TrimString(MemPool_Category memPool, const char* str);
