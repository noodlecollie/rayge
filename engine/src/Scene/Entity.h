#pragma once

#include "Scene/Component.h"

struct RayGE_Entity
{
	bool isInUse;
	RayGE_ComponentHeader* componentList;
	size_t componentCount;
};

void RayGE_AcquireEntity(RayGE_Entity* entity);
void RayGE_ReleaseEntity(RayGE_Entity* entity);
