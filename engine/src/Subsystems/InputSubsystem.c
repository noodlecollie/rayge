#include <string.h>
#include "Subsystems/InputSubsystem.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Subsystems/LoggingSubsystem.h"

// You've got 10 digits on your hands, anything more than
// this is probably an unreasonable expectation for a human
#define MAX_SIMULTANEOUS_KEYS 10

typedef enum KeyState
{
	KEYSTATE_RELEASED = 0,
	KEYSTATE_PRESSED
} KeyState;

typedef struct InputCommandItem
{
	struct InputCommandItem* next;
	RayGE_InputCommand command;
} InputCommandItem;

typedef struct InputCommandCategory
{
	struct InputCommandCategory* next;
	KeyboardKey key;
	InputCommandItem* items;
} InputCommandCategory;

typedef struct Data
{
	InputCommandCategory* categories;

	int pressedKeys[2][MAX_SIMULTANEOUS_KEYS];
	int* keysThisFrame;
	int* keysLastFrame;
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

	FreeCategoryChain(data->categories);
	MEMPOOL_FREE(data);
}

static void SwapKeyBufferPointers(Data* data)
{
	int* temp = data->keysLastFrame;
	data->keysLastFrame = data->keysThisFrame;
	data->keysThisFrame = temp;
}

static InputCommandCategory* FindCategoryForKey(InputCommandCategory* head, KeyboardKey key)
{
	while ( head )
	{
		if ( head->key == key )
		{
			return head;
		}

		head = head->next;
	}

	return NULL;
}

static InputCommandCategory* FindOrCreateCategoryForKey(InputCommandCategory** head, KeyboardKey key)
{
	InputCommandCategory* found = FindCategoryForKey(*head, key);

	if ( !found )
	{
		found = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, InputCommandCategory);
		found->key = key;
		found->next = *head;
		*head = found;
	}

	return found;
}

static void ExecuteCommand(KeyboardKey key, KeyState state)
{
	InputCommandCategory* category = FindCategoryForKey(g_Data->categories, key);

	if ( !category )
	{
		return;
	}

	for ( InputCommandItem* item = category->items; item; item = item->next )
	{
		if ( state == KEYSTATE_PRESSED && item->command.PressFunction )
		{
			item->command.PressFunction();
		}
		else if ( state == KEYSTATE_RELEASED && item->command.ReleaseFunction )
		{
			item->command.ReleaseFunction();
		}
	}
}

static bool KeyIsInList(KeyboardKey key, int* list)
{
	if ( key == KEY_NULL )
	{
		return false;
	}

	for ( int index = 0; index < MAX_SIMULTANEOUS_KEYS; ++index )
	{
		if ( list[index] == key )
		{
			return true;
		}
	}

	return false;
}

static void SwapAndClearKeys(Data* data)
{
	SwapKeyBufferPointers(data);
	memset(data->keysThisFrame, KEY_NULL, MAX_SIMULTANEOUS_KEYS * sizeof(int));
}

static void BufferKeysThisFrame(Data* data)
{
	SwapAndClearKeys(data);

	size_t nextIndex = 0;

	for ( int pressedKey = GetKeyPressed(); pressedKey != KEY_NULL; pressedKey = GetKeyPressed() )
	{
		if ( nextIndex >= MAX_SIMULTANEOUS_KEYS )
		{
			LoggingSubsystem_PrintLine(
				RAYGE_LOG_WARNING,
				"Exceeded max simultaneous keys pressed for key %d",
				pressedKey
			);

			continue;
		}

		data->keysThisFrame[nextIndex++] = pressedKey;
	}
}

static void InvokeForKeysNoLongerPresent(int* lastList, int* currentList, KeyState stateToInvokeIfDifferent)
{
	for ( size_t index = 0; index < MAX_SIMULTANEOUS_KEYS; ++index )
	{
		int key = lastList[index];

		if ( key == KEY_NULL )
		{
			// Reached end
			break;
		}

		if ( !KeyIsInList(key, currentList) )
		{
			ExecuteCommand(key, stateToInvokeIfDifferent);
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
	g_Data->keysThisFrame = g_Data->pressedKeys[0];
	g_Data->keysLastFrame = g_Data->pressedKeys[1];
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

	InputCommandCategory* category = FindOrCreateCategoryForKey(&g_Data->categories, command.key);

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
	InvokeForKeysNoLongerPresent(g_Data->keysLastFrame, g_Data->keysThisFrame, KEYSTATE_RELEASED);
	InvokeForKeysNoLongerPresent(g_Data->keysThisFrame, g_Data->keysLastFrame, KEYSTATE_PRESSED);
}

void InputSubsystem_ReleaseAllKeys(void)
{
	if ( !g_Data )
	{
		return;
	}

	SwapAndClearKeys(g_Data);
	InvokeForKeysNoLongerPresent(g_Data->keysLastFrame, g_Data->keysThisFrame, KEYSTATE_RELEASED);
}
