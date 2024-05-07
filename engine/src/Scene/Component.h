#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "RayGE/SceneTypes.h"

typedef struct RayGE_ComponentHeader
{
	RayGE_ComponentType type;
	struct RayGE_ComponentHeader* next;
} RayGE_ComponentHeader;

#define CHECK_COMPONENT_STRUCTURE(type) \
	static_assert(offsetof(type, header) == 0, "Header must be the first member in the component struct")

typedef struct RayGE_ComponentImpl_Spatial
{
	RayGE_ComponentHeader header;
	RayGE_Component_Spatial data;
} RayGE_ComponentImpl_Spatial;

CHECK_COMPONENT_STRUCTURE(RayGE_ComponentImpl_Spatial);

typedef struct RayGE_ComponentImpl_Camera
{
	RayGE_ComponentHeader header;
	RayGE_Component_Camera data;
} RayGE_ComponentImpl_Camera;

CHECK_COMPONENT_STRUCTURE(RayGE_ComponentImpl_Camera);

void Component_FreeList(RayGE_ComponentHeader* head);

RayGE_ComponentImpl_Spatial* Component_CreateSpatial(void);
RayGE_ComponentImpl_Camera* Component_CreateCamera(void);

// Do not use this function directly - use the macros below instead.
// If ensureTypeMatches is set, emits a fatal log error if the type does not match.
void* Component_CastImpl(
	RayGE_ComponentHeader* header,
	RayGE_ComponentType toType,
	bool ensureTypeMatches,
	const char* file,
	int line
);

// The macros below cast from a component header to an implementation struct:

#define COMPONENTCAST_SPATIAL(header, mustSucceed) \
	((RayGE_ComponentImpl_Spatial*) \
		 Component_CastImpl((header), RAYGE_COMPONENTTYPE_SPATIAL, (mustSucceed), __FILE__, __LINE__))

#define COMPONENTCAST_CAMERA(header, mustSucceed) \
	((RayGE_ComponentImpl_Camera*) \
		 Component_CastImpl((header), RAYGE_COMPONENTTYPE_CAMERA, (mustSucceed), __FILE__, __LINE__))

// The macros below cast from a component header to a data struct:

#define GET_COMPONENT_DATA_PTR(header, typeName) (&COMPONENTCAST_##typeName(header, true)->data)

#define COMPONENTDATA_SPATIAL(header) GET_COMPONENT_DATA_PTR(header, SPATIAL)
#define COMPONENTDATA_CAMERA(header) GET_COMPONENT_DATA_PTR(header, CAMERA)