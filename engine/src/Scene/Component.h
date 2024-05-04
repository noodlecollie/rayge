#pragma once

#include <stdbool.h>
#include "RayGE/SceneTypes.h"

typedef struct RayGE_ComponentHeader
{
	RayGE_ComponentType type;
	struct RayGE_ComponentHeader* next;
} RayGE_ComponentHeader;

typedef struct RayGE_ComponentImpl_Spatial
{
	RayGE_ComponentHeader header;
	RayGE_Component_Spatial data;
} RayGE_ComponentImpl_Spatial;

typedef struct RayGE_ComponentImpl_Camera
{
	RayGE_ComponentHeader header;
	RayGE_Component_Camera data;
} RayGE_ComponentImpl_Camera;

void RayGE_Component_FreeList(RayGE_ComponentHeader* head);

RayGE_ComponentImpl_Spatial* RayGE_Component_CreateSpatial(void);
RayGE_ComponentImpl_Camera* RayGE_Component_CreateCamera(void);

// Do not use this function directly - use the macros below instead.
// If ensureTypeMatches is set, emits a fatal log error if the type does not match.
void* RayGE_Component_CastImpl(
	RayGE_ComponentHeader* header,
	RayGE_ComponentType toType,
	bool ensureTypeMatches,
	const char* file,
	int line
);

#define CMPTCAST_SPATIAL(header, mustSucceed) \
	((RayGE_ComponentImpl_Spatial*) \
		 RayGE_Component_CastImpl((header), RAYGE_COMPONENTTYPE_SPATIAL, (mustSucceed), __FILE__, __LINE__))

#define CMPTCAST_CAMERA(header, mustSucceed) \
	((RayGE_ComponentImpl_Camera*) \
		 RayGE_Component_CastImpl((header), RAYGE_COMPONENTTYPE_CAMERA, (mustSucceed), __FILE__, __LINE__))
