#include "Resources/TextureResources.h"
#include "Logging/Logging.h"
#include "Resources/ResourceHandleUtils.h"
#include "EngineSubsystems/FilesystemSubsystem.h"
#include "MemPool/MemPoolManager.h"
#include "Utils/Utils.h"
#include "wzl_cutl/string.h"
#include "raylib.h"

// These must be powers of two to divide nicely
#define MAX_TEXTURES 2048
#define TEXTURE_BATCH_SIZE 32

typedef struct StringBounds
{
	const char* begin;
	const char* end;
} StringBounds;

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

static StringBounds BoundString(const char* str)
{
	RAYGE_ASSERT_VALID(str);

	StringBounds bounds = {NULL, NULL};
	wzl_strtrimspace(str, &bounds.begin, &bounds.end);

	return bounds;
}

static char* NewStringFromBounds(StringBounds bounds)
{
	RAYGE_ASSERT(bounds.begin && bounds.end && bounds.begin <= bounds.end, "String bounds were not valid");

	const size_t size = (bounds.end - bounds.begin + 1) * sizeof(char);
	char* out = MEMPOOL_MALLOC(MEMPOOL_RESOURCE_MANAGEMENT, size);

	if ( size > 0 )
	{
		memcpy(out, bounds.begin, size - 1);
	}

	out[size - 1] = '\0';
	return out;
}

// If source image is provided and data is valid, it is used as the image for the texture.
// If source image is provided but empty, the texture is loaded off disk and the image
// is updated to hold the data.
// If source image is not provided, the texture is loaded off disk and the image data
// is not kept on the CPU afterwards.
static RayGE_ResourceHandle LoadTextureFromPath(const char* path, Image* sourceImage, bool isInternal)
{
	RAYGE_ASSERT_VALID(g_ResourceList);

	if ( !path || !(*path) )
	{
		return RAYGE_NULL_RESOURCE_HANDLE;
	}

	if ( ResourceList_ItemCount(g_ResourceList) >= MAX_TEXTURES )
	{
		Logging_PrintLine(
			RAYGE_LOG_ERROR,
			"Cannot load texture %s: reached maximum of %d textures",
			path,
			MAX_TEXTURES
		);

		return RAYGE_NULL_RESOURCE_HANDLE;
	}

	char* fullPath = NULL;
	char* trimmedPath = NewStringFromBounds(BoundString(path));
	RayGE_ResourceHandle outHandle = RAYGE_NULL_RESOURCE_HANDLE;

	do
	{
		// Only internal textures loaded from a provided image may be prefixed with ':'
		if ( *trimmedPath == ':' && !isInternal )
		{
			Logging_PrintLine(
				RAYGE_LOG_ERROR,
				"Cannot load texture %s: path prefix ':' is reserved for internal textures",
				trimmedPath
			);

			break;
		}

		ResourceListErrorCode result = ResourceList_CreateNewItem(g_ResourceList, trimmedPath, &outHandle);

		if ( result == RESOURCELIST_ERROR_PATH_ALREADY_EXISTED )
		{
			// Handle will refer to item that existed,
			// so we can just return it.
			break;
		}

		if ( result != RESOURCELIST_ERROR_NONE )
		{
			if ( result == RESOURCELIST_ERROR_NO_FREE_SPACE )
			{
				Logging_PrintLine(
					RAYGE_LOG_ERROR,
					"Cannot load texture %s: reached maximum of %d textures",
					trimmedPath,
					MAX_TEXTURES
				);
			}
			else
			{
				Logging_PrintLine(
					RAYGE_LOG_ERROR,
					"Failed to load texture %s: could not create resource list item (error code: %d)",
					trimmedPath,
					result
				);
			}

			break;
		}

		TextureItem* item = (TextureItem*)ResourceList_GetItemData(g_ResourceList, outHandle);
		RAYGE_ASSERT_VALID(item);

		if ( sourceImage && sourceImage->data )
		{
			Logging_PrintLine(RAYGE_LOG_TRACE, "Loading texture %s from image", trimmedPath);
			item->texture = LoadTextureFromImage(*sourceImage);
		}
		else
		{
			fullPath = FilesystemSubsystem_MakeAbsoluteAlloc(trimmedPath);

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

		if ( item->texture.id == 0 )
		{
			Logging_PrintLine(RAYGE_LOG_ERROR, "Failed to load texture %s", trimmedPath);
			ResourceList_DestroyItem(g_ResourceList, outHandle);
			outHandle = RAYGE_NULL_RESOURCE_HANDLE;
			break;
		}
	}
	while ( false );

	if ( fullPath )
	{
		MEMPOOL_FREE(fullPath);
	}

	if ( trimmedPath )
	{
		MEMPOOL_FREE(trimmedPath);
	}

	return outHandle;
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
