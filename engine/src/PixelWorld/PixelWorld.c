#include "PixelWorld/PixelWorld.h"
#include "MemPool/MemPoolManager.h"
#include "Debugging.h"

#define MEMPOOL_PIXELWORLD MEMPOOL_SCENE

struct PixelWorld
{
	// TODO
	int dummy;
};

PixelWorld* PixelWorld_Create(void)
{
	PixelWorld* world = MEMPOOL_CALLOC_STRUCT(MEMPOOL_PIXELWORLD, PixelWorld);
	return world;
}

void PixelWorld_Destroy(PixelWorld* world)
{
	RAYGE_ASSERT_VALID(world);

	if ( !world )
	{
		return;
	}

	MEMPOOL_FREE(world);
}
