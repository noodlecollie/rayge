#include <string.h>
#include <stdlib.h>
#include "Platform/String.h"

char* Platform_DuplicateString(const char* in)
{
	if ( !in )
	{
		return NULL;
	}

	size_t length = strlen(in) + 1;
	char* out = malloc(length);

	// Two-step just to make absolutely sure this is terminated:
	memcpy(out, in, length - 1);
	out[length] = '\0';

	return out;
}
