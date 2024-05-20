#pragma once

#include <stdint.h>
#include "RayGE/InterfaceUtils.h"

typedef struct RayGE_ResourceHandle
{
	uint32_t domain;
	uint32_t index;
	uint64_t key;
} RayGE_ResourceHandle;

// The first of these is valid in constant assignments.
// The second (according to the -pedantic flag) is not.
// An invalid resource handle will never refer to a
// valid resource.
#define RAYGE_INIT_INVALID_RESOURCE_HANDLE {0, 0, 0}
#define RAYGE_INVALID_RESOURCE_HANDLE (RAYGE_TYPE_LITERAL(RayGE_ResourceHandle) RAYGE_INIT_INVALID_RESOURCE_HANDLE)
