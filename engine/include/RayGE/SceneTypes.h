#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "RayGE/Math.h"
#include "RayGE/Angles.h"
#include "RayGE/InterfaceUtils.h"

typedef struct RayGE_EntityHandle
{
	size_t index;
	uint64_t key;
} RayGE_EntityHandle;

// The first of these is valid in constant assignments.
// The second (according to the -pedantic flag) is not.
#define RAYGE_INIT_INVALID_ENT_HANDLE {~((size_t)0), 0}
#define RAYGE_INVALID_ENT_HANDLE (RAYGE_TYPE_LITERAL(RayGE_EntityHandle) RAYGE_INIT_INVALID_ENT_HANDLE)

typedef uint64_t RayGE_RenderableHandle;

typedef enum RayGE_ComponentType
{
	RAYGE_COMPONENTTYPE_SPATIAL = 0,
	RAYGE_COMPONENTTYPE_CAMERA,
	RAYGE_COMPONENTTYPE_RENDERABLE,
} RayGE_ComponentType;

typedef struct RayGE_Component_Spatial
{
	Vector3 position;
	EulerAngles angles;
} RayGE_Component_Spatial;

typedef struct RayGE_Component_Camera
{
	float fieldOfView;
} RayGE_Component_Camera;

typedef struct RayGE_Component_Renderable
{
	RayGE_RenderableHandle handle;
} RayGE_Component_Renderable;

// Computes whether a handle is valid.
// This does not necessarily mean the entity that it
// refers to is valid (it may have been freed by the engine).
// However, an invalid handle will never refer
// to a valid entity.
static inline bool RayGE_EntityHandleIsValid(RayGE_EntityHandle handle)
{
	return handle.key != 0;
}
