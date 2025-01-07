#include <stddef.h>
#include <string.h>
#include "UI/DeveloperConsole.h"
#include "Logging/Logging.h"
#include "MemPool/MemPoolManager.h"
#include "Debugging.h"
#include "wzl_cutl/math.h"
#include "cimgui.h"

#define LOG_MESSAGE_LIST_CAPACITY 512
#define INDICATOR_ERROR 'E'
#define INDICATOR_WARNING 'W'
#define INDICATOR_INFO 'I'

typedef struct LogMessage
{
	size_t length;  // Not including terminator
	char* message;
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

static void AddLogMessage(Data* data, char* message, size_t length)
{
	if ( data->logMessageCount >= LOG_MESSAGE_LIST_CAPACITY )
	{
		ShiftLogMessages(data);
	}

	LogMessage* entry = &data->logMessageList[data->logMessageCount++];
	entry->message = message;
	entry->length = length;
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

	// +1 for terminator, +1 for level indicator.
	char* messageBuffer = MEMPOOL_MALLOC(MEMPOOL_UI, length + 2);

	switch ( level )
	{
		case RAYGE_LOG_FATAL:
		case RAYGE_LOG_ERROR:
		{
			messageBuffer[0] = INDICATOR_ERROR;
			break;
		}

		case RAYGE_LOG_WARNING:
		{
			messageBuffer[0] = INDICATOR_WARNING;
			break;
		}

		default:
		{
			messageBuffer[0] = INDICATOR_INFO;
		}
	}

	memcpy(messageBuffer + 1, message, length + 1);
	AddLogMessage(data, messageBuffer, length + 1);
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
				igTextWrapped("%s", message->message + 1);
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
