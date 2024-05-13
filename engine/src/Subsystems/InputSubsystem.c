#include "Subsystems/InputSubsystem.h"
#include "Subsystems/MemPoolSubsystem.h"

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

void InputSubsystem_Init(void)
{
	if ( g_Data )
	{
		return;
	}

	g_Data = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, Data);
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

void InputSubsystem_ExecuteCommand(KeyboardKey key, bool pressed)
{
	if ( !g_Data )
	{
		return;
	}

	InputCommandCategory* category = FindCategoryForKey(g_Data->categories, key);

	if ( !category )
	{
		return;
	}

	for ( InputCommandItem* item = category->items; item; item = item->next )
	{
		if ( pressed && item->command.PressFunction )
		{
			item->command.PressFunction();
		}
		else if ( !pressed && item->command.ReleaseFunction )
		{
			item->command.ReleaseFunction();
		}
	}
}
