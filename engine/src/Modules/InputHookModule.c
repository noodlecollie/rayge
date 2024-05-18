#include "Modules/InputHookModule.h"
#include "Modules/UIModule.h"
#include "Logging/Logging.h"
#include "Input/InputBufferKeyboard.h"
#include "utlist.h"

#define UTHASH_POOLED_MEMPOOL MEMPOOL_INPUT
#include "UTUtils/UTHash_Pooled.h"

typedef struct CallbackInfo
{
	RayGE_InputSource source;
	const RayGE_InputBuffer* buffer;
	bool uiIsOpen;
} CallbackInfo;

typedef struct HookItem
{
	struct HookItem* prev;
	struct HookItem* next;
	RayGE_InputHook hook;
	unsigned int modifierFlags;
} HookItem;

typedef struct HookInputHashItem
{
	UT_hash_handle hh;
	int id;
	HookItem* list;
} HookInputHashItem;

typedef struct Data
{
	HookInputHashItem* inputHash[INPUT_SOURCE__COUNT];
} Data;

static Data* g_Data = NULL;

static HookInputHashItem* CreateInputHashItem(int id)
{
	HookInputHashItem* item = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, HookInputHashItem);
	item->id = id;
	return item;
}

static void DestroyInputHashItem(HookInputHashItem* item)
{
	if ( !item )
	{
		return;
	}

	HookItem* deletedElement = NULL;
	HookItem* temp = NULL;

	DL_FOREACH_SAFE(item->list, deletedElement, temp)
	{
		DL_DELETE(item->list, deletedElement);
		MEMPOOL_FREE(deletedElement);
	}

	MEMPOOL_FREE(item);
}

static Data* CreateData(void)
{
	Data* data = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, Data);
	return data;
}

static void DestroyData(Data* data)
{
	if ( !data )
	{
		return;
	}

	for ( size_t index = 0; index < INPUT_SOURCE__COUNT; ++index )
	{
		HookInputHashItem* item = NULL;
		HookInputHashItem* temp = NULL;

		HASH_ITER(hh, data->inputHash[index], item, temp)
		{
			HASH_DEL(data->inputHash[index], item);
			DestroyInputHashItem(item);
		}
	}

	MEMPOOL_FREE(data);
}

static bool
PassesModifierChecks(RayGE_InputSource source, const RayGE_InputBuffer* inputBuffer, unsigned int requiredModifiers)
{
	if ( source != INPUT_SOURCE_KEYBOARD )
	{
		// We don't support any other checks yet.
		return true;
	}

	if ( requiredModifiers == 0 )
	{
		// No need to check anything.
		return true;
	}

	// Get the current modifiers.
	RayGE_KeyboardModifiers currentModifiers = InputBuffer_GetCurrentKeyboardModifiers(inputBuffer);

	// Restrict the modifiers to only the ones we need to check.
	currentModifiers &= requiredModifiers;

	// Toggle all the required modifiers.
	currentModifiers ^= requiredModifiers;

	// If there are any bits enabled, this means not all modifiers were present.
	return currentModifiers == 0;
}

static void CheckAndCallHook(int id, RayGE_InputState state, void* userData)
{
	const CallbackInfo* cbInfo = (const CallbackInfo*)userData;

	// Find any lists for this input.
	HookInputHashItem* hashItem = NULL;
	HASH_FIND_INT(g_Data->inputHash[cbInfo->source], &id, hashItem);

	if ( !hashItem )
	{
		return;
	}

	HookItem* hook = NULL;

	DL_FOREACH(hashItem->list, hook)
	{
		bool shouldTrigger = ((hook->hook.triggerFlags & INPUT_TRIGGER_ACTIVE) && state == INPUT_STATE_ACTIVE) ||
			((hook->hook.triggerFlags & INPUT_TRIGGER_INACTIVE) && state == INPUT_STATE_INACTIVE);

		if ( cbInfo->uiIsOpen && !(hook->hook.triggerFlags & INPUT_TRIGGER_OVERRIDE_UI_FOCUS) )
		{
			shouldTrigger = false;
		}

		// Only apply modifier checks for an active input.
		// For one that triggers on inactivity, we don't care about the modifiers.
		if ( shouldTrigger && state == INPUT_STATE_ACTIVE &&
			 !PassesModifierChecks(cbInfo->source, cbInfo->buffer, hook->modifierFlags) )
		{
			shouldTrigger = false;
		}

		if ( shouldTrigger )
		{
			hook->hook.callback(cbInfo->source, id, cbInfo->buffer, hook->hook.userData);
		}
	}
}

void InputHookModule_Init(void)
{
	if ( g_Data )
	{
		return;
	}

	g_Data = CreateData();
}

void InputHookModule_ShutDown(void)
{
	if ( !g_Data )
	{
		return;
	}

	DestroyData(g_Data);
	g_Data = NULL;
}

void InputHookModule_AddHook(RayGE_InputSource source, int id, unsigned int modifierFlags, RayGE_InputHook hook)
{
	RAYGE_ASSERT_VALID(g_Data);

	if ( !g_Data )
	{
		return;
	}

	if ( source >= INPUT_SOURCE__COUNT )
	{
		Logging_PrintLine(RAYGE_LOG_ERROR, "Invalid input source provided when adding input hook");
		return;
	}

	if ( !hook.callback )
	{
		Logging_PrintLine(RAYGE_LOG_ERROR, "Invalid callback provided when adding input hook");
		return;
	}

	Logging_PrintLine(
		RAYGE_LOG_TRACE,
		"Adding input hook for source %d input %d with modifier condition 0x%08x",
		source,
		id,
		modifierFlags
	);

	HookInputHashItem* hashItem = NULL;
	HASH_FIND_INT(g_Data->inputHash[source], &id, hashItem);

	if ( !hashItem )
	{
		hashItem = CreateInputHashItem(id);
		HASH_ADD_INT(g_Data->inputHash[source], id, hashItem);
	}

	HookItem* hookItem = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, HookItem);
	hookItem->hook = hook;
	hookItem->modifierFlags = modifierFlags;
	DL_APPEND(hashItem->list, hookItem);
}

void InputHookModule_ProcessInput(void)
{
	RAYGE_ASSERT_VALID(g_Data);

	if ( !g_Data )
	{
		return;
	}

	const bool uiIsOpen = UIModule_HasCurrentMenu();

	// Deal with all newly inactive inputs before all newly active ones.
	for ( size_t source = 0; source < INPUT_SOURCE__COUNT; ++source )
	{
		const RayGE_InputSource inputSource = (RayGE_InputSource)source;

		const RayGE_InputBuffer* buffer = InputModule_GetInputForSource(inputSource);
		RAYGE_ENSURE(buffer, "Expected valid input buffer for source");

		const CallbackInfo cbInfo = {
			inputSource,
			buffer,
			uiIsOpen,
		};

		InputBuffer_TriggerForAllInputsNowInactive(buffer, &CheckAndCallHook, (void*)&cbInfo);
	}

	for ( size_t source = 0; source < INPUT_SOURCE__COUNT; ++source )
	{
		const RayGE_InputSource inputSource = (RayGE_InputSource)source;

		const RayGE_InputBuffer* buffer = InputModule_GetInputForSource(inputSource);
		RAYGE_ENSURE(buffer, "Expected valid input buffer for source");

		const CallbackInfo cbInfo = {
			inputSource,
			buffer,
			uiIsOpen,
		};

		InputBuffer_TriggerForAllInputsNowActive(buffer, &CheckAndCallHook, (void*)&cbInfo);
	}
}
