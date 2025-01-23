#include "Resources/PixelWorldResources.h"
#include "Resources/ResourceList.h"
#include "Resources/ResourceDomains.h"
#include "PixelWorld/PixelWorld.h"
#include "Debugging.h"

#define MAX_PIXEL_WORLDS 64
#define ITEMS_PER_BUCKET 8

typedef struct PixelWorldItem
{
	PixelWorld* world;
} PixelWorldItem;

static ResourceList* g_ResourceList = NULL;

static void DeinitItem(void* item)
{
	PixelWorldItem* pItem = (PixelWorldItem*)item;

	if ( pItem->world )
	{
		PixelWorld_Destroy(pItem->world);
		pItem->world = NULL;
	}
}

void PixelWorldResources_Init(void)
{
	if ( g_ResourceList )
	{
		return;
	}

	ResourceListAttributes attributes = (ResourceListAttributes)
	{
		.domain = RESOURCE_DOMAIN_PIXEL_WORLD,
		.itemsPerBucket = ITEMS_PER_BUCKET,
		.maxCapacity = MAX_PIXEL_WORLDS,
		.itemSizeInBytes = sizeof(PixelWorldItem),
		.DeinitItem = &DeinitItem,
	};

	g_ResourceList = ResourceList_Create(attributes);

	RAYGE_ENSURE(g_ResourceList, "Failed to create pixel world resource list!");
}

void PixelWorldResources_ShutDown(void)
{
	if ( !g_ResourceList )
	{
		return;
	}

	ResourceList_Destroy(g_ResourceList);
	g_ResourceList = NULL;
}
