#include <string.h>
#include "Scene/Entity.h"
#include "Scene/Component.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Debugging.h"

void RayGE_AcquireEntity(RayGE_Entity* entity)
{
	if ( !entity )
	{
		return;
	}

	RAYGE_ASSERT(!entity->isInUse, "Entity was already in use");

	memset(entity, 0, sizeof(*entity));
	entity->isInUse = true;
}

void RayGE_ReleaseEntity(RayGE_Entity* entity)
{
	if ( !entity )
	{
		return;
	}

	RAYGE_ASSERT(entity->isInUse, "Entity was not in use");

	RayGE_Component_FreeList(entity->componentList);
	entity->componentList = NULL;
	entity->componentCount = 0;

	entity->isInUse = false;
}
