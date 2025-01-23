#include "JSON/JSONUtils.h"
#include "Logging/Logging.h"
#include "EngineSubsystems/FilesystemSubsystem.h"
#include "MemPool/MemPoolManager.h"

static cJSON* GetItem(
	const char* context,
	cJSON* parent,
	const char* key,
	const char* typeName,
	cJSON_bool (*validator)(const cJSON* const)
)
{
	if ( !parent )
	{
		Logging_PrintLine(RAYGE_LOG_ERROR, "%s object was null.", context ? context : "JSON");
		return NULL;
	}

	cJSON* item = cJSON_GetObjectItem(parent, key);

	if ( !item )
	{
		Logging_PrintLine(
			RAYGE_LOG_ERROR,
			"%s object did not contain expected item \"%s\".",
			context ? context : "JSON",
			key ? key : ""
		);

		return NULL;
	}

	if ( !(*validator)(item) )
	{
		Logging_PrintLine(
			RAYGE_LOG_ERROR,
			"%s object item \"%s\" was not of type \"%s\".",
			context ? context : "JSON",
			key ? key : "",
			typeName
		);

		return NULL;
	}

	return item;
}

cJSON* JSONUtils_ExpectObjectItem(const char* context, cJSON* parent, const char* key)
{
	return GetItem(context, parent, key, "object", &cJSON_IsObject);
}

cJSON* JSONUtils_ExpectStringItem(const char* context, cJSON* parent, const char* key)
{
	return GetItem(context, parent, key, "string", &cJSON_IsString);
}

cJSON* JSONUtils_LoadFromFile(const char* relPath)
{
	char* fullPath = FilesystemSubsystem_MakeAbsoluteAlloc(relPath);

	size_t length = 0;
	uint8_t* fileData = FilesystemSubsystem_LoadFileData(fullPath, &length);

	cJSON* json = cJSON_ParseWithLength((const char*)fileData, length);

	if ( fileData )
	{
		FilesystemSubsystem_UnloadFileData(fileData);
	}

	if ( fullPath )
	{
		MEMPOOL_FREE(fullPath);
	}

	if ( !json )
	{
		Logging_PrintLine(RAYGE_LOG_ERROR, "Failed to load JSON from %s", relPath);
	}

	return json;
}
