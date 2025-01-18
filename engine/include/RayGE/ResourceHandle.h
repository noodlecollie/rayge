#pragma once

#include <stdint.h>
#include "RayGE/InterfaceUtils.h"

typedef struct RayGE_ResourceHandle
{
	// Domain (or general type) of the resource being referred to.
	uint32_t domain;

	// Unique index of this resource.
	uint32_t index;

	// Specific key for this resource. Used to verify that
	// this resource is the one we want, and the handle is
	// not an old one from a previous resource that used
	// to live at the same index.
	uint64_t key;
} RayGE_ResourceHandle;

// The first of these is valid in constant assignments.
// The second (according to the -pedantic flag) is not.
// A normal handle may not refer to a valid resource if,
// for example, the resource it used to refer to has been
// freed, but a null handle is guaranteed to never
// be used to refer to any resource.
#define RAYGE_INIT_NULL_RESOURCE_HANDLE {0, 0, 0}
#define RAYGE_NULL_RESOURCE_HANDLE (RAYGE_TYPE_LITERAL(RayGE_ResourceHandle) RAYGE_INIT_NULL_RESOURCE_HANDLE)
#define RAYGE_IS_NULL_RESOURCE_HANDLE(handle) ((handle).domain == 0 && (handle).index == 0 && (handle).key == 0)
