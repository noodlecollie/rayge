#include "EngineSubsystems/ResourceSubsystem.h"
#include "Resources/TextureResources.h"
#include "Resources/PixelWorldResources.h"
#include "Utils/Utils.h"

typedef struct InitAndShutdown
{
	void (*Init)(void);
	void (*ShutDown)(void);
} InitAndShutdown;

static const InitAndShutdown g_Resources[] = {
	{TextureResources_Init, TextureResources_ShutDown},
	{PixelWorldResources_Init, PixelWorldResources_ShutDown},
};

static bool g_Initialised = false;

void ResourceSubsystem_Init(void)
{
	if ( g_Initialised )
	{
		return;
	}

	for ( size_t index = 0; index < RAYGE_ARRAY_SIZE(g_Resources); ++index )
	{
		if ( g_Resources[index].Init )
		{
			g_Resources[index].Init();
		}
	}

	g_Initialised = true;
}

void ResourceSubsystem_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	for ( size_t index = RAYGE_ARRAY_SIZE(g_Resources); index > 0; --index )
	{
		if ( g_Resources[index - 1].ShutDown )
		{
			g_Resources[index - 1].ShutDown();
		}
	}

	g_Initialised = false;
}
