#pragma once

// These are the internal domains that we have control of.
// The RESOURCEFLAG_INTERNAL_DOMAIN flag is applied to
// handles that refer to resources within one of these domains,
// so that if we decide to support handles that refer to
// other resources in domains we don't control, we can do
// that by not adding the internal flag to the handle.
typedef enum InternalResourceDomain
{
	RESOURCE_DOMAIN_INVALID = 0,
	RESOURCE_DOMAIN_ENTITY,
	RESOURCE_DOMAIN_RENDERABLE_PRIMITIVE,
	RESOURCE_DOMAIN_TEXTURE,
	RESOURCE_DOMAIN_PIXEL_WORLD,

	RESOURCE_DOMAIN__COUNT
} InternalResourceDomain;

#define RESOURCEFLAG_INTERNAL_DOMAIN (1 << 31)
#define RESOURCE_DOMAIN_ID_MASK ((uint32_t)0x7FFFFFFF)
