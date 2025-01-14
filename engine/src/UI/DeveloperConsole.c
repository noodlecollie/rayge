#include <stddef.h>
#include <string.h>
#include <float.h>
#include "UI/DeveloperConsole.h"
#include "Logging/Logging.h"
#include "MemPool/MemPoolManager.h"
#include "Commands/CommandParser.h"
#include "Debugging.h"
#include "wzl_cutl/math.h"
#include "cimgui.h"

#define LOG_MESSAGE_LIST_CAPACITY 512
#define MAX_COMMAND_LENGTH 256

typedef struct LogMessage
{
	RayGE_Log_Level level;
	char* message;
	size_t length;  // Not including terminator
} LogMessage;

typedef struct Data
{
	bool show;
	bool justShown;
	LogMessage* logMessageList;
	size_t logMessageCount;
	size_t lastLogMessageIndex;
	bool scrollToBottom;
	char commandInputBuffer[MAX_COMMAND_LENGTH];
} Data;

static Data g_Data;
static bool g_Initialised = false;

static void FreeLogMessageContents(LogMessage* message)
{
	if ( message->message )
	{
		MEMPOOL_FREE(message->message);
	}

	memset(message, 0, sizeof(*message));
}

static void DeleteAllLogMessages(Data* data)
{
	for ( size_t index = 0; index < data->logMessageCount; ++index )
	{
		FreeLogMessageContents(&data->logMessageList[index]);
	}

	data->logMessageCount = 0;
}

static void AddLogMessage(Data* data, RayGE_Log_Level level, char* message, size_t length)
{
	if ( data->logMessageCount < LOG_MESSAGE_LIST_CAPACITY )
	{
		data->lastLogMessageIndex = data->logMessageCount++;
	}
	else
	{
		data->lastLogMessageIndex = (data->lastLogMessageIndex + 1) % LOG_MESSAGE_LIST_CAPACITY;
	}

	LogMessage* entry = &data->logMessageList[data->lastLogMessageIndex];

	if ( data->logMessageCount >= LOG_MESSAGE_LIST_CAPACITY )
	{
		FreeLogMessageContents(entry);
	}

	entry->message = message;
	entry->length = length;
	entry->level = level;
}

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

static int ConsoleInputCallback(ImGuiInputTextCallbackData* data)
{
	// Nothing here for now, but this callback must be provided
	// or ImGui complains. We will probably do command completion here.
	(void)data;
	return 0;
}

static void ExecuteCurrentCommand(char* command)
{
	if ( !command || !command[0] )
	{
		return;
	}

	CommandParser_ParseAndExecute(command);
	command[0] = '\0';
}

static void DrawLogMessage(const LogMessage* message)
{
	if ( !message->message )
	{
		return;
	}

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

static void Init(void* userData)
{
	Data* data = (Data*)userData;

	if ( g_Initialised )
	{
		return;
	}

	data->commandInputBuffer[0] = '\0';
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
	data->commandInputBuffer[0] = '\0';

	MEMPOOL_FREE(data->logMessageList);
	data->logMessageList = NULL;

	g_Initialised = false;
}

static void Show(void* userData)
{
	Data* data = (Data*)userData;

	data->show = true;
	data->justShown = true;
}

static void Hide(void* userData)
{
	Data* data = (Data*)userData;

	data->show = false;
	data->justShown = false;
}

static bool Poll(void* userData)
{
	Data* data = (Data*)userData;

	if ( !data->show )
	{
		return false;
	}

	igSetNextWindowPos((ImVec2) {10.0f, 10.0f}, ImGuiCond_FirstUseEver, (ImVec2) {0.0f, 0.0f});

	igSetNextWindowSize(
		(ImVec2) {0.5f * igGetMainViewport()->WorkSize.x, 0.75f * igGetMainViewport()->WorkSize.y},
		ImGuiCond_FirstUseEver
	);

	igSetNextWindowSizeConstraints((ImVec2) {300.0f, 300.0f}, (ImVec2) {FLT_MAX, FLT_MAX}, NULL, NULL);

	if ( igBegin("Developer Console", &data->show, ImGuiWindowFlags_NoCollapse) )
	{
		if ( igBeginChild_Str(
				 "ScrollingRegion",
				 (ImVec2) {0, -(igGetFrameHeight() + (3.0f * igGetStyle()->ItemSpacing.y))},
				 ImGuiChildFlags_NavFlattened,
				 ImGuiWindowFlags_AlwaysVerticalScrollbar
			 ) )
		{
			// TODO: We could optimise the following so we don't draw things that
			// are way off the top of the text area.
			// Note however that this might be made difficult by wrapped lines!
			// Perhaps we need a table, since you can query whether a row has been clipped?

			// All the messages that exist in the list in front of the last one
			// we added (so all the really old messages):
			for ( size_t index = data->lastLogMessageIndex + 1; index < LOG_MESSAGE_LIST_CAPACITY; ++index )
			{
				DrawLogMessage(&data->logMessageList[index]);
			}

			// Then everything from the beginning to our last message:
			for ( size_t index = 0; index <= data->lastLogMessageIndex; ++index )
			{
				DrawLogMessage(&data->logMessageList[index]);
			}

			if ( data->scrollToBottom || igGetScrollY() >= igGetScrollMaxY() )
			{
				igSetScrollHereY(1.0f);
			}

			data->scrollToBottom = false;
		}

		igEndChild();
		igSeparator();
		igSpacing();

		bool reclaimFocus = data->justShown;
		const ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue |
			ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion |
			ImGuiInputTextFlags_CallbackHistory;

		if ( igInputTextEx(
				 "##",
				 "",
				 data->commandInputBuffer,
				 sizeof(data->commandInputBuffer),
				 (ImVec2) {-100.0f, igGetFrameHeight()},
				 inputFlags,
				 ConsoleInputCallback,
				 data
			 ) )
		{
			ExecuteCurrentCommand(data->commandInputBuffer);
			reclaimFocus = true;
		}

		igSetItemDefaultFocus();

		if ( reclaimFocus )
		{
			igSetKeyboardFocusHere(-1);
		}

		igSameLine(0.0f, -1.0f);

		if ( igButton("Submit", (ImVec2) {-0.1f, igGetFrameHeight()}) )
		{
			ExecuteCurrentCommand(data->commandInputBuffer);
		}
	}

	igEnd();

	data->justShown = false;
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
