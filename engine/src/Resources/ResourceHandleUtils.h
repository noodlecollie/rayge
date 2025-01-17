#pragma once

#include <stdbool.h>
#include "RayGE/ResourceHandle.h"
#include "Resources/ResourceDomains.h"
#include "Debugging.h"

RayGE_ResourceHandle Resource_CreateInternalHandle(InternalResourceDomain domain, uint32_t index, uint64_t key);
uint64_t Resource_CreateKey(uint32_t index);
InternalResourceDomain Resource_GetInternalDomain(RayGE_ResourceHandle handle);

bool Resource_HandleIsValidForInternalDomain(
	RayGE_ResourceHandle handle,
	InternalResourceDomain domain,
	uint32_t maxResourceCount
);
