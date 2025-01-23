#include "Resources/TextureResources.h"
#include "Logging/Logging.h"
#include "Resources/ResourceHandleUtils.h"
#include "EngineSubsystems/FilesystemSubsystem.h"
#include "MemPool/MemPoolManager.h"
#include "Resources/ResourceListUtils.h"
#include "Utils/Utils.h"
#include "Utils/StringUtils.h"
#include "wzl_cutl/string.h"
#include "raylib.h"

// These must be powers of two to divide nicely
#define MAX_TEXTURES 2048
#define TEXTURE_BATCH_SIZE 32

typedef struct TextureItem
{
	Texture2D texture;
} TextureItem;

static ResourceList* g_ResourceList = NULL;

static void DeinitItem(void* item)
{
	TextureItem* texItem = (TextureItem*)item;

	if ( texItem->texture.id != 0 )
	{
		UnloadTexture(texItem->texture);
		texItem->texture.id = 0;
	}
}

static bool CreateItemCallback(const char* relPath, void* itemData, void* userData)
{
	TextureItem* item = (TextureItem*)itemData;
	Image* sourceImage = (Image*)userData;
	char* fullPath = NULL;

	do
	{
		if ( sourceImage && sourceImage->data )
		{
			Logging_PrintLine(RAYGE_LOG_TRACE, "Loading texture %s from image", relPath);
			item->texture = LoadTextureFromImage(*sourceImage);
		}
		else
		{
			fullPath = FilesystemSubsystem_MakeAbsoluteAlloc(relPath);

			if ( sourceImage )
			{
				Logging_PrintLine(RAYGE_LOG_TRACE, "Loading texture %s from file and retaining source image", fullPath);
				*sourceImage = LoadImage(fullPath);

				if ( !sourceImage->data )
				{
					break;
				}

				item->texture = LoadTextureFromImage(*sourceImage);
			}
			else
			{
				Logging_PrintLine(RAYGE_LOG_TRACE, "Loading texture %s from file", fullPath);
				item->texture = LoadTexture(fullPath);
			}
		}
	}
	while ( false );

	if ( fullPath )
	{
		MEMPOOL_FREE(fullPath);
	}

	return item->texture.id != 0;
}

// If source image is provided and data is valid, it is used as the image for the texture.
// If source image is provided but empty, the texture is loaded off disk and the image
// is updated to hold the data.
// If source image is not provided, the texture is loaded off disk and the image data
// is not kept on the CPU afterwards.
static RayGE_ResourceHandle LoadTextureFromPath(const char* path, Image* sourceImage, bool isInternal)
{
	RAYGE_ASSERT_VALID(g_ResourceList);
	RAYGE_ASSERT(path && *path, "Expected a valid path");

	if ( !g_ResourceList || !path || !(*path) )
	{
		return RAYGE_NULL_RESOURCE_HANDLE;
	}

	StringBounds bounds = StringUtils_GetStringTrimBounds(path);

	if ( *bounds.begin == ':' && !isInternal )
	{
		Logging_PrintLine(
			RAYGE_LOG_ERROR,
			"Cannot load texture %s: path prefix ':' is reserved for internal textures",
			path
		);

		return RAYGE_NULL_RESOURCE_HANDLE;
	}

	return ResourceListUtils_CreateNewItem("texture", g_ResourceList, path, CreateItemCallback, sourceImage);

}

static void UnloadTextureFromHandle(RayGE_ResourceHandle handle, bool requestIsInternal)
{
	RAYGE_ASSERT_VALID(g_ResourceList);

	// Only bother running the extra check if the handle is not null.
	// Null handles are just silently ignored.
	if ( !RAYGE_IS_NULL_RESOURCE_HANDLE(handle) )
	{
		RAYGE_ASSERT_EXPECT(
			Resource_GetInternalDomain(handle) == RESOURCE_DOMAIN_TEXTURE,
			"Cannot unload texture from handle that does not refer to a texture!"
		);
	}

	// Catches the null case too:
	if ( Resource_GetInternalDomain(handle) != RESOURCE_DOMAIN_TEXTURE )
	{
		return;
	}

	const char* path = ResourceList_GetItemPath(g_ResourceList, handle);

	if ( path && (path[0] == ':') != requestIsInternal )
	{
		Logging_PrintLine(
			RAYGE_LOG_ERROR,
			"Ignoring request to unload %s texture %s from %s call",
			requestIsInternal ? "non-internal" : "internal",
			path,
			requestIsInternal ? "an internal" : "a non-internal"
		);

		return;
	}

	if ( !ResourceList_DestroyItem(g_ResourceList, handle) )
	{
		Logging_PrintLine(
			RAYGE_LOG_WARNING,
			"Could not unload texture: provided handle did not refer to a loaded texture"
		);
	}
}

void TextureResources_Init(void)
{
	if ( g_ResourceList )
	{
		return;
	}

	ResourceListAttributes atts;
	memset(&atts, 0, sizeof(atts));

	atts.domain = RESOURCE_DOMAIN_TEXTURE;
	atts.maxCapacity = MAX_TEXTURES;
	atts.itemsPerBucket = TEXTURE_BATCH_SIZE;
	atts.itemSizeInBytes = sizeof(TextureItem);
	atts.DeinitItem = &DeinitItem;

	g_ResourceList = ResourceList_Create(atts);

	RAYGE_ENSURE(g_ResourceList, "Failed to create texture resource list!");
}

void TextureResources_ShutDown(void)
{
	if ( !g_ResourceList )
	{
		return;
	}

	ResourceList_Destroy(g_ResourceList);
	g_ResourceList = NULL;
}

RayGE_ResourceHandle TextureResources_LoadTexture(const char* path)
{
	return LoadTextureFromPath(path, NULL, false);
}

RayGE_ResourceHandle TextureResources_LoadTextureAndRetainImage(const char* path, Image* outImage)
{
	RAYGE_ASSERT(outImage && !outImage->data, "Output image is not valid");

	if ( !outImage )
	{
		return RAYGE_NULL_RESOURCE_HANDLE;
	}

	memset(outImage, 0, sizeof(*outImage));
	return LoadTextureFromPath(path, outImage, false);
}

RayGE_ResourceHandle TextureResources_LoadInternalTexture(const char* name, Image sourceImage)
{
	RAYGE_ASSERT_VALID(name && *name);
	RAYGE_ASSERT_VALID(sourceImage.data);

	if ( !name || !(*name) || !sourceImage.data )
	{
		return RAYGE_NULL_RESOURCE_HANDLE;
	}

	char internalName[64];
	int charsWritten = wzl_sprintf(internalName, sizeof(internalName), ":%s", name);

	RAYGE_ENSURE(charsWritten > 0, "Failed to construct texture's internal name");
	RAYGE_ENSURE((size_t)charsWritten < sizeof(internalName), "Texture's internal name was too long");

	return LoadTextureFromPath(internalName, &sourceImage, true);
}

void TextureResources_UnloadTexture(RayGE_ResourceHandle handle)
{
	UnloadTextureFromHandle(handle, false);
}

void TextureResources_UnloadInternalTexture(RayGE_ResourceHandle handle)
{
	UnloadTextureFromHandle(handle, true);
}

void TextureResources_UnloadAll(void)
{
	Logging_PrintLine(RAYGE_LOG_DEBUG, "Unloading all texture resources");

	if ( g_ResourceList )
	{
		ResourceList_Destroy(g_ResourceList);
		g_ResourceList = NULL;
	}
}

size_t TextureResources_NumTextures(void)
{
	RAYGE_ASSERT_VALID(g_ResourceList);
	return ResourceList_ItemCount(g_ResourceList);
}

const ResourceList* TestureResources_GetResourceList(void)
{
	RAYGE_ASSERT_VALID(g_ResourceList);
	return g_ResourceList;
}

Texture2D TextureResources_GetTexture(ResourceListIterator iterator)
{
	RAYGE_ASSERT_VALID(g_ResourceList);

	TextureItem* item = (TextureItem*)ResourceList_GetItemDataFromIterator(iterator);
	return item ? item->texture : (Texture2D) {0, 0, 0, 0, 0};
}

const char* TextureResources_GetPath(ResourceListIterator iterator)
{
	RAYGE_ASSERT_VALID(g_ResourceList);
	return ResourceList_GetItemPathFromIterator(iterator);
}
