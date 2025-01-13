#include "Rendering/TextureResources.h"
#include "Logging/Logging.h"
#include "ResourceManagement/ResourceHandleUtils.h"
#include "Utils.h"
#include "wzl_cutl/string.h"
#include "raylib.h"

#define UTHASH_POOLED_MEMPOOL MEMPOOL_RESOURCE_MANAGEMENT
#include "UTUtils/UTHash_Pooled.h"

// These must be powers of two to divide nicely
#define MAX_TEXTURES 2048
#define TEXTURE_BATCH_SIZE 32
#define NUM_TEXTURE_BATCHES (MAX_TEXTURES / TEXTURE_BATCH_SIZE)

typedef struct StringBounds
{
	const char* begin;
	const char* end;
} StringBounds;

typedef struct PathToResourceHandleHashItem
{
	UT_hash_handle hh;
	char* path;
	RayGE_ResourceHandle handle;
} PathToResourceHandleHashItem;

typedef struct TextureEntry
{
	Texture2D texture;
	uint64_t key;
	PathToResourceHandleHashItem* hashItem;
} TextureEntry;

typedef struct TextureBatch
{
	TextureEntry textures[TEXTURE_BATCH_SIZE];
	bool isNotEmpty;
	bool isFull;
} TextureBatch;

typedef struct Data
{
	PathToResourceHandleHashItem* pathToResourceHandle;
	TextureBatch** batches;
	size_t totalTextures;
} Data;

static Data g_Data;

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
};

static void DeleteBatch(TextureBatch* batch)
{
	if ( !batch )
	{
		return;
	}

	for ( size_t index = 0; index < RAYGE_ARRAY_SIZE(batch->textures); ++index )
	{
		TextureEntry* texture = &batch->textures[index];

		if ( texture->texture.id != 0 )
		{
			UnloadTexture(texture->texture);
		}
	}

	MEMPOOL_FREE(batch);
}

static void DeleteAllBatches(Data* data)
{
	if ( !data->batches )
	{
		return;
	}

	for ( size_t index = 0; index < NUM_TEXTURE_BATCHES; ++index )
	{
		DeleteBatch(data->batches[index]);
	}

	MEMPOOL_FREE(data->batches);
	data->batches = NULL;
	data->totalTextures = 0;
}

static void DeleteHashEntry(Data* data, PathToResourceHandleHashItem* item)
{
	HASH_DEL(data->pathToResourceHandle, item);
	MEMPOOL_FREE(item->path);
	MEMPOOL_FREE(item);
}

static void DeleteAllHashEntries(Data* data)
{
	PathToResourceHandleHashItem* item = NULL;
	PathToResourceHandleHashItem* temp = NULL;

	HASH_ITER(hh, data->pathToResourceHandle, item, temp)
	{
		DeleteHashEntry(data, item);
	}
}

static RayGE_ResourceHandle FindTextureHandleByPath(const char* path)
{
	PathToResourceHandleHashItem* item = NULL;
	HASH_FIND_STR(g_Data.pathToResourceHandle, path, item);

	return item ? item->handle : RAYGE_NULL_RESOURCE_HANDLE;
}

static size_t GetFirstAvailableBatchIndex(Data* data)
{
	if ( !data->batches )
	{
		data->batches =
			(TextureBatch**)MEMPOOL_CALLOC(MEMPOOL_RESOURCE_MANAGEMENT, NUM_TEXTURE_BATCHES, sizeof(TextureBatch*));
	}

	for ( size_t index = 0; index < NUM_TEXTURE_BATCHES; ++index )
	{
		if ( !data->batches[index] )
		{
			data->batches[index] = MEMPOOL_CALLOC_STRUCT(MEMPOOL_RESOURCE_MANAGEMENT, TextureBatch);
		}

		if ( !data->batches[index]->isFull )
		{
			return index;
		}
	}

	return SIZE_MAX;
}

static void UpdateBatchFlags(TextureBatch* batch)
{
	batch->isFull = true;
	batch->isNotEmpty = false;

	for ( size_t index = 0; index < RAYGE_ARRAY_SIZE(batch->textures); ++index )
	{
		if ( batch->textures->texture.id == 0 )
		{
			batch->isFull = false;
		}
		else
		{
			batch->isNotEmpty = true;
		}

		if ( !batch->isFull && batch->isNotEmpty )
		{
			// We can quit now - there will be no more changes to any flags.
			return;
		}
	}
}

static void ClearTextureEntry(Data* data, TextureEntry* entry)
{
	if ( !entry )
	{
		return;
	}

	UnloadTexture(entry->texture);
	DeleteHashEntry(data, entry->hashItem);

	memset(entry, 0, sizeof(*entry));
}

static RayGE_ResourceHandle AddTextureToBatch(Data* data, TextureEntry entryToAdd, bool isInternal)
{
	size_t batchIndex = GetFirstAvailableBatchIndex(data);

	// We shouldn't have been called if this was going to happen:
	RAYGE_ENSURE(batchIndex < NUM_TEXTURE_BATCHES, "Overflowed max textures! This should never happen!");

	TextureBatch* batch = data->batches[batchIndex];

	size_t index = 0;
	for ( ; index < RAYGE_ARRAY_SIZE(batch->textures); ++index )
	{
		TextureEntry* entry = &batch->textures[index];

		if ( entry->texture.id == 0 )
		{
			*entry = entryToAdd;
			break;
		}
	}

	RAYGE_ENSURE(index < RAYGE_ARRAY_SIZE(batch->textures), "Overflowed texture batch! This should never happen!");

	UpdateBatchFlags(batch);

	return isInternal ? Resource_CreateInternalHandle(RESOURCE_DOMAIN_TEXTURE, (uint32_t)batchIndex, entryToAdd.key)
					  : Resource_CreateHandle(RESOURCE_DOMAIN_TEXTURE, (uint32_t)batchIndex, entryToAdd.key);
}

static RayGE_ResourceHandle AddTextureToResourceList(Data* data, char* path, Texture2D texture)
{
	RAYGE_ASSERT(data->totalTextures < MAX_TEXTURES, "Expected total textures to be < MAX_TEXTURES");

	PathToResourceHandleHashItem* item =
		MEMPOOL_CALLOC_STRUCT(MEMPOOL_RESOURCE_MANAGEMENT, PathToResourceHandleHashItem);

	const TextureEntry entry = {
		.texture = texture,
		.key = Resource_CreateKey((uint32_t)data->totalTextures),
		.hashItem = item,
	};

	const RayGE_ResourceHandle handle = AddTextureToBatch(&g_Data, entry, path[0] == ':');

	item->path = path;
	item->handle = handle;

	HASH_ADD_STR(g_Data.pathToResourceHandle, path, item);

	++data->totalTextures;
	return handle;
}

static TextureEntry* FindTextureEntryByHandle(Data* data, RayGE_ResourceHandle handle, TextureBatch** outBatch)
{
	if ( handle.index >= NUM_TEXTURE_BATCHES )
	{
		return NULL;
	}

	TextureBatch* batch = data->batches[handle.index];

	if ( !batch )
	{
		return NULL;
	}

	for ( size_t index = 0; index < RAYGE_ARRAY_SIZE(batch->textures); ++index )
	{
		TextureEntry* entry = &batch->textures[index];

		if ( entry->texture.id != 0 && entry->key == handle.key )
		{
			if ( outBatch )
			{
				*outBatch = batch;
			}

			return entry;
		}
	}

	return NULL;
}

static TextureResources_Iterator CreateInvalidIterator(void)
{
	return (TextureResources_Iterator) {
		.batchIndex = NUM_TEXTURE_BATCHES,
		.entryIndex = TEXTURE_BATCH_SIZE,
	};
}

static bool IteratorRefersToBatch(Data* data, const TextureResources_Iterator* iterator)
{
	return iterator && data->batches && iterator->batchIndex < NUM_TEXTURE_BATCHES &&
		data->batches[iterator->batchIndex];
}

static bool IncrementIteratorToNextValidEntry(TextureResources_Iterator* iterator, const TextureBatch* batch)
{
	do
	{
		++iterator->entryIndex;
	}
	while ( iterator->entryIndex < RAYGE_ARRAY_SIZE(batch->textures) &&
			batch->textures[iterator->entryIndex].texture.id == 0 );

	return iterator->entryIndex < RAYGE_ARRAY_SIZE(batch->textures);
}

static bool IncrementIteratorToNextValidBatch(Data* data, TextureResources_Iterator* iterator)
{
	RAYGE_ASSERT_VALID(data && data->batches);

	iterator->entryIndex = 0;

	do
	{
		++iterator->batchIndex;
	}
	while ( iterator->batchIndex < NUM_TEXTURE_BATCHES &&
			(!data->batches[iterator->batchIndex] || !(data->batches[iterator->batchIndex]->isNotEmpty)) );

	return iterator->batchIndex < NUM_TEXTURE_BATCHES;
}

static TextureEntry* GetEntryFromIterator(Data* data, TextureResources_Iterator* iterator)
{
	if ( !IteratorRefersToBatch(data, iterator) || iterator->entryIndex >= TEXTURE_BATCH_SIZE )
	{
		return NULL;
	}

	TextureBatch* batch = data->batches[iterator->batchIndex];
	return &batch->textures[iterator->entryIndex];
};

static RayGE_ResourceHandle LoadTextureFromPath(const char* path, const Image* sourceImage)
{
	if ( !path || !(*path) )
	{
		return RAYGE_NULL_RESOURCE_HANDLE;
	}

	if ( g_Data.totalTextures >= MAX_TEXTURES )
	{
		Logging_PrintLine(
			RAYGE_LOG_ERROR,
			"Cannot load texture %s: reached maximum of %zu textures",
			path,
			g_Data.totalTextures
		);

		return RAYGE_NULL_RESOURCE_HANDLE;
	}

	char* trimmedPath = NewStringFromBounds(BoundString(path));
	RayGE_ResourceHandle outHandle = RAYGE_NULL_RESOURCE_HANDLE;

	do
	{
		// Only internal textures loaded from a provided image may be prefixed with ':'
		if ( *trimmedPath == ':' && !sourceImage )
		{
			Logging_PrintLine(
				RAYGE_LOG_ERROR,
				"Cannot load texture %s: path prefix is reserved for internal textures",
				trimmedPath
			);

			break;
		}

		RayGE_ResourceHandle existingTexture = FindTextureHandleByPath(trimmedPath);

		if ( !RAYGE_IS_NULL_RESOURCE_HANDLE(existingTexture) )
		{
			// The texture already existed, so just return its existing handle.
			outHandle = existingTexture;
			break;
		}

		Texture2D texture = sourceImage ? LoadTextureFromImage(*sourceImage) : LoadTexture(trimmedPath);

		if ( texture.id == 0 )
		{
			Logging_PrintLine(RAYGE_LOG_ERROR, "Failed to load texture %s", trimmedPath);
			break;
		}

		// This will take ownership of the path, so we null it afterwards.
		outHandle = AddTextureToResourceList(&g_Data, trimmedPath, texture);
		trimmedPath = NULL;
	}
	while ( false );

	if ( trimmedPath )
	{
		// Path was not consumed, so must be freed.
		MEMPOOL_FREE(trimmedPath);
	}

	return outHandle;
}

static void UnloadTextureFromHandle(RayGE_ResourceHandle handle)
{
	TextureBatch* batch = NULL;
	TextureEntry* entry = FindTextureEntryByHandle(&g_Data, handle, &batch);

	if ( !entry )
	{
		return;
	}

	ClearTextureEntry(&g_Data, entry);
	batch->isFull = false;
}

RayGE_ResourceHandle TextureResources_LoadTexture(const char* path)
{
	return LoadTextureFromPath(path, NULL);
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

	return LoadTextureFromPath(internalName, &sourceImage);
}

void TextureResources_UnloadTexture(RayGE_ResourceHandle handle)
{
	if ( Resource_HandleIsInternal(handle) )
	{
		RAYGE_ASSERT(false, "Cannot unload internal texture here");
		Logging_PrintLine(RAYGE_LOG_ERROR, "Ignoring attempt to unload internal texture");
		return;
	}

	UnloadTextureFromHandle(handle);
}

void TextureResources_UnloadInternalTexture(RayGE_ResourceHandle handle)
{
	if ( !Resource_HandleIsInternal(handle) )
	{
		RAYGE_ASSERT(false, "Cannot unload non-internal texture here");
		Logging_PrintLine(RAYGE_LOG_ERROR, "Ignoring attempt to unload non-internal texture");
		return;
	}

	UnloadTextureFromHandle(handle);
}

void TextureResources_UnloadAll(void)
{
	Logging_PrintLine(RAYGE_LOG_DEBUG, "Unloading all texture resources");

	DeleteAllHashEntries(&g_Data);
	DeleteAllBatches(&g_Data);
}

size_t TextureResources_NumTextures(void)
{
	return g_Data.totalTextures;
}

TextureResources_Iterator TextureResources_CreateBeginIterator(void)
{
	if ( !g_Data.batches || g_Data.totalTextures < 1 )
	{
		return CreateInvalidIterator();
	}

	TextureResources_Iterator iterator = {
		.batchIndex = 0,
		.entryIndex = 0,
	};

	if ( IteratorRefersToBatch(&g_Data, &iterator) )
	{
		TextureBatch* batch = g_Data.batches[iterator.batchIndex];

		if ( iterator.entryIndex < RAYGE_ARRAY_SIZE(batch->textures) &&
			 batch->textures[iterator.entryIndex].texture.id != 0 )
		{
			// No need to increment.
			return iterator;
		}
	}

	// Increment the iterator to the first element before we return it.
	// If there is no first entry, it will just end up invalid.
	TextureResources_IncrementIterator(&iterator);
	return iterator;
}

TextureResources_Iterator TextureResources_CreateIterator(RayGE_ResourceHandle handle)
{
	TextureBatch* batch = NULL;
	TextureEntry* entry = FindTextureEntryByHandle(&g_Data, handle, &batch);

	if ( !batch || !entry )
	{
		return CreateInvalidIterator();
	}

	return (TextureResources_Iterator) {
		.batchIndex = handle.index,
		.entryIndex = entry - batch->textures,
	};
}

bool TextureResources_IsIteratorValid(TextureResources_Iterator iterator)
{
	TextureEntry* entry = GetEntryFromIterator(&g_Data, &iterator);
	return entry && entry->texture.id != 0;
}

bool TextureResources_IncrementIterator(TextureResources_Iterator* iterator)
{
	if ( !IteratorRefersToBatch(&g_Data, iterator) )
	{
		false;
	}

	bool nextBatchValid = false;

	do
	{
		TextureBatch* batch = g_Data.batches[iterator->batchIndex];

		if ( IncrementIteratorToNextValidEntry(iterator, batch) )
		{
			return true;
		}

		nextBatchValid = IncrementIteratorToNextValidBatch(&g_Data, iterator);

		if ( nextBatchValid )
		{
			batch = g_Data.batches[iterator->batchIndex];

			if ( batch->textures[iterator->entryIndex].texture.id != 0 )
			{
				return true;
			}
		}
	}
	while ( nextBatchValid );

	return false;
}

Texture2D TextureResourcesIterator_GetTexture(TextureResources_Iterator iterator)
{
	TextureEntry* entry = GetEntryFromIterator(&g_Data, &iterator);
	return entry ? entry->texture : (Texture2D) {0, 0, 0, 0, 0};
}

const char* TextureResourcesIterator_GetPath(TextureResources_Iterator iterator)
{
	TextureEntry* entry = GetEntryFromIterator(&g_Data, &iterator);
	return entry ? entry->hashItem->path : NULL;
}
