#include <stdint.h>
#include <stdbool.h>
#include "BehaviouralSubsystems/BSysManager.h"
#include "BehaviouralSubsystems/SpatialBSys.h"
#include "BehaviouralSubsystems/RenderableBSys.h"
#include "Utils/Utils.h"
#include "Debugging.h"

static const BSys_Definition* g_Subsystems[] =
{
	&SpatialBSys_Definition,
	&RenderableBSys_Definition,
};

static bool g_Initialised = false;

void BSysManager_Init(void)
{
	if ( g_Initialised )
	{
		return;
	}

	const BSys_Definition** end = g_Subsystems + RAYGE_ARRAY_SIZE(g_Subsystems);

	for ( const BSys_Definition** subsystem = g_Subsystems; subsystem < end; ++subsystem )
	{
		RAYGE_ENSURE_VALID(*subsystem);

		if ( (*subsystem)->Init )
		{
			(*subsystem)->Init();
		}
	}

	g_Initialised = true;
}

void BSysManager_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	for ( const BSys_Definition** subsystem = g_Subsystems + RAYGE_ARRAY_SIZE(g_Subsystems) - 1;
		  subsystem >= g_Subsystems;
		  --subsystem )
	{
		RAYGE_ENSURE_VALID(*subsystem);
		RAYGE_ENSURE((*subsystem)->Invoke, "BSys missing invocation function!");

		if ( (*subsystem)->ShutDown )
		{
			(*subsystem)->ShutDown();
		}
	}

	g_Initialised = false;
}

void BSysManager_Invoke(BSys_Stage stage)
{
	RAYGE_ENSURE(g_Initialised, "BSys manager must be initialised before invoking a stage!");

	if ( !g_Initialised )
	{
		return;
	}

	const uint64_t stageFlag = BSYS_STAGE_FLAG(stage);

	for ( size_t index = 0; index < RAYGE_ARRAY_SIZE(g_Subsystems); ++index )
	{
		const BSys_Definition* subsystem = g_Subsystems[index];

		if ( !(subsystem->stageMask & stageFlag) )
		{
			continue;
		}

		RAYGE_ENSURE(subsystem->Invoke, "BSys missing invocation function!");

		if ( !subsystem->Invoke )
		{
			continue;
		}

		subsystem->Invoke(stage);
	}
}
