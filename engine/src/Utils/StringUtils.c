#include <stddef.h>
#include "Utils/StringUtils.h"
#include "Debugging.h"
#include "wzl_cutl/string.h"

StringBounds StringUtils_GetStringTrimBounds(const char* str)
{
	StringBounds bounds = {NULL, NULL};

	if ( str )
	{
		wzl_strtrimspace(str, &bounds.begin, &bounds.end);
	}

	return bounds;
}

char* StringUtils_TrimString(MemPool_Category memPool, const char* str)
{
	if ( !str )
	{
		return NULL;
	}

	StringBounds bounds = StringUtils_GetStringTrimBounds(str);
	RAYGE_ASSERT(bounds.begin && bounds.end && bounds.begin <= bounds.end, "Expected string bounds to be valid");

	const size_t size = ((size_t)(bounds.end - bounds.begin) + 1) * sizeof(char);
	char* out = MEMPOOL_MALLOC(memPool, size);

	if ( size > 0 )
	{
		memcpy(out, bounds.begin, size - 1);
	}

	out[size - 1] = '\0';
	return out;
}
