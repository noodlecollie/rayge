#include <string.h>
#include "Game/GameData.h"
#include "RayGE/Platform.h"
#include "cJSON.h"
#include "JSON/JSONUtils.h"
#include "Subsystems/FileSubsystem.h"
#include "Subsystems/LoggingSubsystem.h"
#include "wzl_cutl/string.h"
#include "Utils.h"

#if RAYGE_PLATFORM() == RAYGE_PLATFORM_LINUX
#define GAME_JSON_KEY_PLATFORM_NAME "linux"
#elif RAYGE_PLATFORM() == RAYGE_PLATFORM_WINDOWS
#define GAME_JSON_KEY_PLATFORM_NAME "windows"
#endif

typedef struct GameData
{
	FileSubsystem_Path clientLibrary;
} GameData;

static GameData g_GameData;
static bool g_IsLoaded = false;

static cJSON* ParseJSONFromFile(const char* path)
{
	size_t size = 0;
	uint8_t* fileData = FileSubsystem_LoadFileData(path, &size);

	if ( !fileData )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Could not load game JSON from %s", path);
		return NULL;
	}

	cJSON* out = cJSON_ParseWithLength((const char*)fileData, size);

	if ( !out )
	{
		// cJSON is not the best at reporting errors,
		// but not sure what else we can really do here...
		const char* errorPtr = cJSON_GetErrorPtr();

		LoggingSubsystem_PrintLine(
			RAYGE_LOG_ERROR,
			"Failed to parse %s. Parse failed at character %zu",
			path,
			errorPtr - (const char*)fileData
		);
	}

	FileSubsystem_UnloadFileData(fileData);
	return out;
}

static bool LoadProp_ClientLibrary(cJSON* json)
{
	cJSON* libItem = JSONUtils_ExpectObjectItem("Game JSON", json, "client_library");

	if ( !libItem )
	{
		return false;
	}

	cJSON* libPathItem = JSONUtils_ExpectStringItem("Game lib config", libItem, GAME_JSON_KEY_PLATFORM_NAME);

	if ( !libPathItem )
	{
		return false;
	}

	wzl_strcpy(g_GameData.clientLibrary, sizeof(g_GameData.clientLibrary), cJSON_GetStringValue(libPathItem));
	return true;
}

static bool LoadDataFromJSON(cJSON* json)
{
	typedef bool (*PropertyLoader)(cJSON*);

	static const PropertyLoader loadFuncs[] =
	{
		LoadProp_ClientLibrary
	};

	bool success = true;

	for ( size_t index = 0; index < RAYGE_ARRAY_SIZE(loadFuncs); ++index )
	{
		if ( !(loadFuncs[index])(json) )
		{
			success = false;
		}
	}

	return success;
}

bool GameData_Load(const char* filePath)
{
	memset(&g_GameData, 0, sizeof(g_GameData));
	g_IsLoaded = false;

	cJSON* json = NULL;
	bool success = false;

	do
	{
		json = ParseJSONFromFile(filePath);

		if ( !json )
		{
			// Error will have been logged.
			break;
		}

		if ( !cJSON_IsObject(json) )
		{
			LoggingSubsystem_PrintLine(RAYGE_LOG_WARNING, "Game JSON root was not an object");
			break;
		}

		success = LoadDataFromJSON(json);
	}
	while ( false );

	if ( json )
	{
		cJSON_Delete(json);
	}

	g_IsLoaded = success;

	if ( g_IsLoaded )
	{
		LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "Successfully loaded data from game JSON");
	}
	else
	{
		memset(&g_GameData, 0, sizeof(g_GameData));
		LoggingSubsystem_PrintLine(RAYGE_LOG_ERROR, "Failed to load data from game JSON");
	}

	return g_IsLoaded;
}

bool GameData_IsLoaded(void)
{
	return g_IsLoaded;
}

const char* GameData_GetGameLibraryPath(void)
{
	return g_GameData.clientLibrary;
}
