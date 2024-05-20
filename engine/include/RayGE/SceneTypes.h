#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "RayGE/Math.h"
#include "RayGE/Angles.h"
#include "RayGE/InterfaceUtils.h"
#include "RayGE/ResourceHandle.h"

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
	RayGE_ResourceHandle handle;
} RayGE_Component_Renderable;
