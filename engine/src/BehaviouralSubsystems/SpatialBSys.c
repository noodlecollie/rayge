#include "BehaviouralSubsystems/SpatialBSys.h"

static void Init(void)
{
	// TODO
}

static void ShutDown(void)
{
	// TODO
}

static void Invoke(BSys_Stage stage)
{
	// TODO
	(void)stage;
}

const BSys_Definition SpatialBSys_Definition =
{
	BSYS_STAGE_FLAG(BSYS_STAGE_DESERIALISATION) | BSYS_STAGE_FLAG(BSYS_STAGE_SERIALISATION),
	Init,
	ShutDown,
	Invoke,
};
