#include "Modules/InputModule.h"
#include "MemPool/MemPoolManager.h"
#include "Logging/Logging.h"
#include "Input/InputBuffer.h"
#include "Input/InputBufferKeyboard.h"
#include "Debugging.h"
#include "raylib.h"

// You've got 10 digits on your hands, anything more than
// this is probably an unreasonable expectation for a human
#define MAX_SIMULTANEOUS_INPUTS 10

typedef struct InputClass
{
	RayGE_InputBuffer* buffer;
	int defaultValue;
} InputClass;

typedef struct InputData
{
	InputClass inputClasses[INPUT_SOURCE__COUNT];
	bool clearAllInputThisFrame;
} InputData;

static InputData* g_Data = NULL;

static InputData* CreateInputData(void)
{
	InputData* data = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, InputData);

	for ( size_t index = 0; index < INPUT_SOURCE__COUNT; ++index )
	{
		data->inputClasses[index].buffer = InputBuffer_Create(MAX_SIMULTANEOUS_INPUTS);
	}

	data->inputClasses[INPUT_SOURCE_KEYBOARD].defaultValue = KEY_NULL;

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

void InputModule_Init(void)
{
	if ( g_Data )
	{
		return;
	}

	g_Data = CreateInputData();
}

void InputModule_ShutDown(void)
{
	if ( !g_Data )
	{
		return;
	}

	DestroyInputData(g_Data);
	g_Data = NULL;
}

void InputModule_ClearAllInputThisFrame(void)
{
	RAYGE_ASSERT_VALID(g_Data);

	if ( !g_Data )
	{
		return;
	}

	g_Data->clearAllInputThisFrame = true;
}

const RayGE_InputBuffer* InputModule_GetInputForSource(RayGE_InputSource source)
{
	RAYGE_ASSERT_VALID(g_Data);
	RAYGE_ASSERT_VALID(source < INPUT_SOURCE__COUNT);

	if ( !g_Data || source >= INPUT_SOURCE__COUNT )
	{
		return NULL;
	}

	return g_Data->inputClasses[source].buffer;
}

void InputModule_NewFrame(void)
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

void InputModule_ProcessInput(void)
{
	RAYGE_ASSERT_VALID(g_Data);

	if ( !g_Data )
	{
		return;
	}

	if ( g_Data->clearAllInputThisFrame )
	{
		InputBuffer_SetCurrentBufferValues(
			g_Data->inputClasses[INPUT_SOURCE_KEYBOARD].buffer,
			g_Data->inputClasses[INPUT_SOURCE_KEYBOARD].defaultValue
		);
	}
	else
	{
		InputBuffer_PopulateFromKeyboard(g_Data->inputClasses[INPUT_SOURCE_KEYBOARD].buffer);
	}
}
