#pragma once

#include <stdbool.h>
#include "RayGE/ResourceHandle.h"
#include "Debugging.h"

typedef enum RayGE_ResourceDomainFlag
{
	RESOURCEFLAG_INTERNAL_DOMAIN = (1 << 31)
} RayGE_ResourceDomainFlag;

typedef enum RayGE_InternalResourceDomain
{
	RESOURCE_DOMAIN_INVALID = 0,
	RESOURCE_DOMAIN_ENTITY = 1,

	RESOURCE_DOMAIN__COUNT
} RayGE_InternalResourceDomain;

#define RESOURCE_DOMAIN_ID_MASK ((uint32_t)0x7FFFFFFF)

RayGE_ResourceHandle Resource_CreateInternalHandle(RayGE_InternalResourceDomain domain, uint32_t index, uint64_t key);
uint64_t Resource_CreateKey(uint32_t index);
RayGE_InternalResourceDomain Resource_GetInternalDomain(RayGE_ResourceHandle handle);

bool Resource_HandleIsValidForInternalDomain(
	RayGE_ResourceHandle handle,
	RayGE_InternalResourceDomain domain,
	uint32_t maxResourceCount
);
