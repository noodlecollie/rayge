#include "PixelWorld/PixelWorld.h"
#include "MemPool/MemPoolManager.h"
#include "Resources/TextureResources.h"
#include "EngineSubsystems/FilesystemSubsystem.h"
#include "JSON/JSONUtils.h"
#include "Debugging.h"
#include "cwalk.h"

#define MEMPOOL_PIXELWORLD MEMPOOL_SCENE

struct PixelWorld
{
	RayGE_ResourceHandle baseTexture;
	Image baseImage;
};

static bool LoadBaseTexture(const char* context, const char* basePath, cJSON* json, PixelWorld* world)
{
	cJSON* baseItem = JSONUtils_ExpectStringItem(context, json, "base");

	if ( !baseItem )
	{
		return false;
	}

	FilesystemSubsystem_Path baseTexturePath;
	wzl_sprintf(baseTexturePath, sizeof(baseTexturePath), "%s/%s", basePath, baseItem->string);

	Logging_PrintLine(RAYGE_LOG_TRACE, "%s Loading base texture %s", context, baseTexturePath);
	world->baseTexture = TextureResources_LoadTextureAndRetainImage(baseTexturePath, &world->baseImage);

	if ( RAYGE_IS_NULL_RESOURCE_HANDLE(world->baseTexture) )
	{
		Logging_PrintLine(RAYGE_LOG_ERROR, "%s Could not load base texture %s", context, baseTexturePath);
		return false;
	}

	return true;
}

static bool ParseJson(const char* jsonPath, PixelWorld* world)
{
	cJSON* json = JSONUtils_LoadFromFile(jsonPath);

	if ( !json )
	{
		return false;
	}

	char context[FILESYSTEM_MAX_REL_PATH + 4];
	wzl_sprintf(context, sizeof(context), "%s:", jsonPath);

	FilesystemSubsystem_Path basePath;
	wzl_strcpy(basePath, sizeof(basePath), jsonPath);

	size_t dirnameLength = 0;
	cwk_path_get_dirname(jsonPath, &dirnameLength);
	RAYGE_ENSURE(dirnameLength < sizeof(basePath), "Unexpected dirname length");

	basePath[dirnameLength] = '\0';

	bool success = false;

	do
	{
		if ( !cJSON_IsObject(json) )
		{
			Logging_PrintLine(RAYGE_LOG_ERROR, "%s Contents are not a JSON object", context);
			break;
		}

		if ( !LoadBaseTexture(context, basePath, json, world) )
		{
			break;
		}

		success = true;
	}
	while ( false );

	cJSON_Delete(json);
	return success;
}

PixelWorld* PixelWorld_Create(const char* jsonPath)
{
	PixelWorld* world = MEMPOOL_CALLOC_STRUCT(MEMPOOL_PIXELWORLD, PixelWorld);

	if ( ParseJson(jsonPath, world) )
	{
		return world;
	}

	PixelWorld_Destroy(world);
	return NULL;
}

void PixelWorld_Destroy(PixelWorld* world)
{
	RAYGE_ASSERT_VALID(world);

	if ( !world )
	{
		return;
	}

	if ( world->baseImage.data )
	{
		UnloadImage(world->baseImage);
	}

	TextureResources_UnloadTexture(world->baseTexture);

	MEMPOOL_FREE(world);
}
