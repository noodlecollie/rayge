#include <stddef.h>
#include <string.h>
#include "UI/DeveloperConsole.h"
#include "Logging/Logging.h"
#include "MemPool/MemPoolManager.h"
#include "Debugging.h"
#include "wzl_cutl/math.h"
#include "cimgui.h"

#define LOG_MESSAGE_LIST_CAPACITY 512

typedef struct LogMessage
{
	RayGE_Log_Level level;
	char* message;
	size_t length;  // Not including terminator
} LogMessage;

typedef struct Data
{
	bool show;
	LogMessage* logMessageList;
	size_t logMessageCount;
} Data;

static Data g_Data;
static bool g_Initialised = false;

static void DeleteAllLogMessages(Data* data)
{
	for ( size_t index = 0; index < data->logMessageCount; ++index )
	{
		LogMessage* message = &data->logMessageList[index];

		if ( message->message )
		{
			MEMPOOL_FREE(message->message);
			message->message = NULL;
			message->length = 0;
		}
	}

	data->logMessageCount = 0;
}

static void ShiftLogMessages(Data* data)
{
	const size_t shift = LOG_MESSAGE_LIST_CAPACITY / 2;

	memmove(
		data->logMessageList,
		data->logMessageList + shift,
		(LOG_MESSAGE_LIST_CAPACITY - shift) * sizeof(LogMessage)
	);

	data->logMessageCount -= WZL_MIN(data->logMessageCount, shift);

	memset(
		data->logMessageList + data->logMessageCount,
		0,
		(LOG_MESSAGE_LIST_CAPACITY - data->logMessageCount) * sizeof(LogMessage)
	);
}

static void AddLogMessage(Data* data, RayGE_Log_Level level, char* message, size_t length)
{
	if ( data->logMessageCount >= LOG_MESSAGE_LIST_CAPACITY )
	{
		ShiftLogMessages(data);
	}

	LogMessage* entry = &data->logMessageList[data->logMessageCount++];
	entry->message = message;
	entry->length = length;
	entry->level = level;
};

static void AcceptLogMessage(RayGE_Log_Level level, const char* message, size_t length, void* userData)
{
	Data* data = (Data*)userData;

	RAYGE_ASSERT_VALID(g_Initialised);
	RAYGE_ASSERT_VALID(message);
	RAYGE_ASSERT_VALID(length > 0);

	if ( !g_Initialised || !message || length < 1 )
	{
		return;
	}

	// +1 for terminator
	char* messageBuffer = MEMPOOL_MALLOC(MEMPOOL_UI, length + 1);
	memcpy(messageBuffer, message, length + 1);

	AddLogMessage(data, level, messageBuffer, length);
}

static bool GetMessageColour(RayGE_Log_Level level, ImVec4* colour)
{
	switch ( level )
	{
		case RAYGE_LOG_FATAL:
		case RAYGE_LOG_ERROR:
		{
			if ( colour )
			{
				*colour = (ImVec4) {1.0f, 0.4f, 0.4f, 1.0f};
			}

			return true;
		}

		case RAYGE_LOG_WARNING:
		{
			if ( colour )
			{
				*colour = (ImVec4) {1.0f, 0.8f, 0.6f, 1.0f};
			}
			return true;
		}

		default:
		{
			return false;
		}
	}
}

static void Init(void* userData)
{
	Data* data = (Data*)userData;

	if ( g_Initialised )
	{
		return;
	}

	data->logMessageList = MEMPOOL_CALLOC(MEMPOOL_UI, LOG_MESSAGE_LIST_CAPACITY, sizeof(LogMessage));
	Logging_AddListener(AcceptLogMessage, data);

	g_Initialised = true;
}

static void ShutDown(void* userData)
{
	if ( !g_Initialised )
	{
		return;
	}

	Data* data = (Data*)userData;

	Logging_RemoveListener(AcceptLogMessage);
	DeleteAllLogMessages(data);

	MEMPOOL_FREE(data->logMessageList);
	data->logMessageList = NULL;

	g_Initialised = false;
}

static void Show(void* userData)
{
	((Data*)userData)->show = true;
}

static void Hide(void* userData)
{
	((Data*)userData)->show = true;
}

static bool Poll(void* userData)
{
	Data* data = (Data*)userData;

	if ( !data->show )
	{
		return false;
	}

	if ( igBegin("Developer Console", &data->show, ImGuiWindowFlags_NoCollapse) )
	{
		if ( igBeginChild_Str("ScrollingRegion", (ImVec2) {0, 0}, ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_None) )
		{
			for ( size_t index = 0; index < data->logMessageCount; ++index )
			{
				const LogMessage* message = &data->logMessageList[index];

				ImVec4 colour = {1.0f, 1.0f, 1.0f, 1.0f};
				const bool hasColour = GetMessageColour(message->level, &colour);

				if ( hasColour )
				{
					igPushStyleColor_Vec4(ImGuiCol_Text, colour);
				}

				igTextWrapped("%s", message->message);

				if ( hasColour )
				{
					igPopStyleColor(1);
				}
			}
		}

		igEndChild();
	}

	igEnd();

	return data->show;
}

const RayGE_UIMenu Menu_DeveloperConsole = {
	&g_Data,

	Init,
	ShutDown,
	Show,
	Hide,
	Poll,
};
