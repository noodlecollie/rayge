#include "EngineSubsystems/InputSubsystem.h"
#include "MemPool/MemPoolManager.h"
#include "Input/InputBuffer.h"
#include "Input/InputBufferKeyboard.h"
#include "Logging//Logging.h"
#include "Debugging.h"
#include "RayGE/APIs/Logging.h"
#include "raylib.h"

// You've got 10 digits on your hands, anything more than
// this is probably an unreasonable expectation for a human
#define MAX_SIMULTANEOUS_INPUTS 10

static const int DEFAULT_VALUES[INPUT_SOURCE__COUNT] = {
	KEY_NULL,  // Keyboard
};

static const char* const LAYER_NAMES[INPUT_LAYER__COUNT] = {
#define LIST_ITEM(enum, name) name,
	RAYGE_INPUT_LAYER_LIST
#undef LIST_ITEM
};

typedef struct InputClass
{
	RayGE_InputBuffer* buffer;
	RayGE_InputBuffer* blankBuffer;
} InputClass;

typedef struct InputData
{
	InputClass inputClasses[INPUT_SOURCE__COUNT];
	bool clearAllInputThisFrame;
	RayGE_InputLayer currentLayer;
} InputData;

static InputData* g_Data = NULL;

static InputData* CreateInputData(void)
{
	InputData* data = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, InputData);
	data->currentLayer = INPUT_LAYER_GAME;

	for ( size_t index = 0; index < INPUT_SOURCE__COUNT; ++index )
	{
		data->inputClasses[index].buffer = InputBuffer_Create(MAX_SIMULTANEOUS_INPUTS);
		data->inputClasses[index].blankBuffer = InputBuffer_Create(MAX_SIMULTANEOUS_INPUTS);

		InputBuffer_SetCurrentBufferValues(data->inputClasses[index].buffer, DEFAULT_VALUES[index]);

		InputBuffer_SetCurrentBufferValues(data->inputClasses[index].blankBuffer, DEFAULT_VALUES[index]);
	}

	return data;
}

static void DestroyInputData(InputData* data)
{
	if ( !data )
	{
		return;
	}

	for ( size_t index = 0; index < INPUT_SOURCE__COUNT; ++index )
	{
		InputBuffer_Destroy(data->inputClasses[index].buffer);
	}

	MEMPOOL_FREE(data);
}

void InputSubsystem_Init(void)
{
	if ( g_Data )
	{
		return;
	}

	g_Data = CreateInputData();
}

void InputSubsystem_ShutDown(void)
{
	if ( !g_Data )
	{
		return;
	}

	DestroyInputData(g_Data);
	g_Data = NULL;
}

void InputSubsystem_ClearAllInputThisFrame(void)
{
	RAYGE_ASSERT_VALID(g_Data);

	if ( !g_Data )
	{
		return;
	}

	g_Data->clearAllInputThisFrame = true;
}

const RayGE_InputBuffer* InputSubsystem_GetInputBuffer(RayGE_InputSource source, RayGE_InputLayer layer)
{
	RAYGE_ASSERT_VALID(g_Data);
	RAYGE_ASSERT_VALID(source < INPUT_SOURCE__COUNT);
	RAYGE_ASSERT_VALID(layer < INPUT_LAYER__COUNT);

	if ( !g_Data || source >= INPUT_SOURCE__COUNT || layer >= INPUT_LAYER__COUNT )
	{
		return NULL;
	}

	const InputClass* inputClass = &g_Data->inputClasses[source];
	return layer == g_Data->currentLayer ? inputClass->buffer : inputClass->blankBuffer;
}

RayGE_InputLayer InputSubsystem_GetCurrentInputLayer(void)
{
	RAYGE_ASSERT_VALID(g_Data);

	return g_Data ? g_Data->currentLayer : INPUT_LAYER_GAME;
}

void InputSubsystem_SetCurrentInputLayer(RayGE_InputLayer layer)
{
	RAYGE_ASSERT_VALID(g_Data);
	RAYGE_ASSERT_VALID(layer < INPUT_LAYER__COUNT);

	if ( !g_Data || layer >= INPUT_LAYER__COUNT || layer == g_Data->currentLayer )
	{
		return;
	}

	Logging_PrintLine(RAYGE_LOG_DEBUG, "Setting input layer: %s", LAYER_NAMES[layer]);
	g_Data->currentLayer = layer;
}

void InputSubsystem_NewFrame(void)
{
	RAYGE_ASSERT_VALID(g_Data);

	if ( !g_Data )
	{
		return;
	}

	g_Data->clearAllInputThisFrame = false;

	for ( size_t index = 0; index < INPUT_SOURCE__COUNT; ++index )
	{
		InputBuffer_Swap(g_Data->inputClasses[index].buffer);
	}
}

void InputSubsystem_ProcessInput(void)
{
	RAYGE_ASSERT_VALID(g_Data);

	if ( !g_Data )
	{
		return;
	}

	if ( g_Data->clearAllInputThisFrame )
	{
		for ( size_t index = 0; index < INPUT_SOURCE__COUNT; ++index )
		{
			InputBuffer_SetCurrentBufferValues(g_Data->inputClasses[index].buffer, DEFAULT_VALUES[index]);
		}
	}
	else
	{
		InputBuffer_PopulateFromKeyboard(g_Data->inputClasses[INPUT_SOURCE_KEYBOARD].buffer);
	}
}
