#include <string.h>
#include "Subsystems/InputSubsystem.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Debugging.h"

// You've got 10 digits on your hands, anything more than
// this is probably an unreasonable expectation for a human
#define MAX_SIMULTANEOUS_INPUTS 10

typedef enum ButtonState
{
	BUTTONSTATE_RELEASED = 0,
	BUTTONSTATE_PRESSED
} ButtonState;

typedef struct InputCommandItem
{
	struct InputCommandItem* next;
	RayGE_InputCommand command;
} InputCommandItem;

typedef struct InputCommandCategory
{
	struct InputCommandCategory* next;
	int id;
	InputCommandItem* items;
} InputCommandCategory;

typedef struct InputBuffer
{
	int activeInputs[2][MAX_SIMULTANEOUS_INPUTS];
	int* lastFrame;
	int* thisFrame;
} InputBuffer;

typedef struct InputClass
{
	RayGE_InputType type;
	InputCommandCategory* commandCategories;
	InputBuffer inputState;
} InputClass;

typedef struct Data
{
	InputClass inputClasses[INPUT_TYPE__COUNT];
} Data;

static Data* g_Data = NULL;

static void FreeItemChain(InputCommandItem* item)
{
	if ( !item )
	{
		return;
	}

	while ( item )
	{
		InputCommandItem* next = item->next;
		MEMPOOL_FREE(item);
		item = next;
	}
}

static void FreeCategoryChain(InputCommandCategory* category)
{
	if ( !category )
	{
		return;
	}

	while ( category )
	{
		InputCommandCategory* next = category->next;
		FreeItemChain(category->items);
		MEMPOOL_FREE(category);
		category = next;
	}
}

static void FreeData(Data* data)
{
	if ( !data )
	{
		return;
	}

	for ( size_t index = 0; index < INPUT_TYPE__COUNT; ++index )
	{
		FreeCategoryChain(data->inputClasses[index].commandCategories);
	}

	MEMPOOL_FREE(data);
}

static void InitBuffer(InputBuffer* buffer)
{
	memset(buffer, 0, sizeof(*buffer));
	buffer->lastFrame = buffer->activeInputs[0];
	buffer->thisFrame = buffer->activeInputs[1];
}

static void SwapBufferPointers(InputBuffer* buffer)
{
	int* temp = buffer->lastFrame;
	buffer->lastFrame = buffer->thisFrame;
	buffer->thisFrame = temp;
}

static void ResetBufferForThisFrame(InputBuffer* buffer, int value)
{
	memset(buffer->thisFrame, value, MAX_SIMULTANEOUS_INPUTS * sizeof(int));
}

static InputCommandCategory* FindCategoryForInput(InputCommandCategory* head, int id)
{
	while ( head )
	{
		if ( head->id == id )
		{
			return head;
		}

		head = head->next;
	}

	return NULL;
}

static InputCommandCategory* FindOrCreateCategoryForInput(InputCommandCategory** head, int id)
{
	InputCommandCategory* found = FindCategoryForInput(*head, id);

	if ( !found )
	{
		found = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, InputCommandCategory);
		found->id = id;
		found->next = *head;
		*head = found;
	}

	return found;
}

static void ExecuteCommand(InputClass* inputClass, int id, ButtonState state)
{
	InputCommandCategory* category = FindCategoryForInput(inputClass->commandCategories, id);

	if ( !category )
	{
		return;
	}

	for ( InputCommandItem* item = category->items; item; item = item->next )
	{
		if ( state == BUTTONSTATE_PRESSED && item->command.PressFunction )
		{
			item->command.PressFunction();
		}
		else if ( state == BUTTONSTATE_RELEASED && item->command.ReleaseFunction )
		{
			item->command.ReleaseFunction();
		}
	}
}

static bool InputIsInList(int id, const int* list)
{
	for ( int index = 0; index < MAX_SIMULTANEOUS_INPUTS; ++index )
	{
		if ( list[index] == id )
		{
			return true;
		}
	}

	return false;
}

static void SwapAndClearKeys(InputBuffer* state, int clearValue)
{
	SwapBufferPointers(state);
	ResetBufferForThisFrame(state, clearValue);
}

static void BufferKeysThisFrame(Data* data)
{
	SwapAndClearKeys(&data->inputClasses[INPUT_TYPE_KEYBOARD].inputState, KEY_NULL);

	size_t nextIndex = 0;

	for ( int pressedKey = GetKeyPressed(); pressedKey != KEY_NULL; pressedKey = GetKeyPressed() )
	{
		if ( nextIndex >= MAX_SIMULTANEOUS_INPUTS )
		{
			LoggingSubsystem_PrintLine(
				RAYGE_LOG_WARNING,
				"Exceeded max simultaneous keys pressed for key %d",
				pressedKey
			);

			continue;
		}

		data->inputClasses[INPUT_TYPE_KEYBOARD].inputState.thisFrame[nextIndex++] = pressedKey;
	}
}

static void InvokeForInputsNoLongerPresent(
	InputClass* inputClass,
	const int* lastList,
	const int* currentList,
	ButtonState stateToInvokeIfDifferent
)
{
	for ( size_t index = 0; index < MAX_SIMULTANEOUS_INPUTS; ++index )
	{
		int key = lastList[index];

		if ( !InputIsInList(key, currentList) )
		{
			ExecuteCommand(inputClass, key, stateToInvokeIfDifferent);
		}
	}
}

void InputSubsystem_Init(void)
{
	if ( g_Data )
	{
		return;
	}

	g_Data = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, Data);

	for ( size_t index = 0; index < INPUT_TYPE__COUNT; ++index )
	{
		g_Data->inputClasses[index].type = (RayGE_InputType)index;
		InitBuffer(&g_Data->inputClasses[index].inputState);
	}
}

void InputSubsystem_ShutDown(void)
{
	if ( !g_Data )
	{
		return;
	}

	FreeData(g_Data);
	g_Data = NULL;
}

void InputSubsystem_RegisterCommand(RayGE_InputCommand command)
{
	if ( !g_Data )
	{
		return;
	}

	RAYGE_ASSERT(command.type < INPUT_TYPE__COUNT, "Unexpected input type value %d", command.type);

	if ( command.type >= INPUT_TYPE__COUNT )
	{
		return;
	}

	InputCommandCategory* category =
		FindOrCreateCategoryForInput(&g_Data->inputClasses[command.type].commandCategories, command.id);

	InputCommandItem* item = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, InputCommandItem);
	item->command = command;
	item->next = category->items;
	category->items = item;
}

void InputSubsystem_ProcessInput(void)
{
	if ( !g_Data )
	{
		return;
	}

	BufferKeysThisFrame(g_Data);

	// Handle releasing existing keys before pressing new ones
	for ( size_t index = 0; index < INPUT_TYPE__COUNT; ++index )
	{
		InvokeForInputsNoLongerPresent(
			&g_Data->inputClasses[index],
			g_Data->inputClasses[index].inputState.lastFrame,
			g_Data->inputClasses[index].inputState.thisFrame,
			BUTTONSTATE_RELEASED
		);
	}

	for ( size_t index = 0; index < INPUT_TYPE__COUNT; ++index )
	{
		InvokeForInputsNoLongerPresent(
			&g_Data->inputClasses[index],
			g_Data->inputClasses[index].inputState.thisFrame,
			g_Data->inputClasses[index].inputState.lastFrame,
			BUTTONSTATE_PRESSED
		);
	}
}

void InputSubsystem_ReleaseAllKeys(void)
{
	if ( !g_Data )
	{
		return;
	}

	SwapAndClearKeys(&g_Data->inputClasses[INPUT_TYPE_KEYBOARD].inputState, KEY_NULL);

	for ( size_t index = 0; index < INPUT_TYPE__COUNT; ++index )
	{
		InvokeForInputsNoLongerPresent(
			&g_Data->inputClasses[index],
			g_Data->inputClasses[index].inputState.lastFrame,
			g_Data->inputClasses[index].inputState.thisFrame,
			BUTTONSTATE_RELEASED
		);
	}
}
