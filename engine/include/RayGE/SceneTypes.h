#pragma once

#include "RayGE/Math.h"
#include "RayGE/Angles.h"

typedef struct RayGE_Entity RayGE_Entity;

typedef enum RayGE_ComponentType
{
	RAYGE_COMPONENTTYPE_SPATIAL = 0,
	RAYGE_COMPONENTTYPE_CAMERA,
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
