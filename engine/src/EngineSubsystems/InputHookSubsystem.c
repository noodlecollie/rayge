#include "EngineSubsystems/InputHookSubsystem.h"
#include "Logging/Logging.h"
#include "Input/InputBufferKeyboard.h"
#include "utlist.h"
#include "Headless.h"

#define UTHASH_POOLED_MEMPOOL MEMPOOL_INPUT
#include "UTUtils/UTHash_Pooled.h"

#if !RAYGE_HEADLESS()
#include "EngineSubsystems/UISubsystem.h"
#endif

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

static HookItem* CreateHookItem(unsigned int modifierFlags, RayGE_InputHook hook)
{
	HookItem* item = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, HookItem);

	item->hook = hook;
	item->modifierFlags = modifierFlags;

	return item;
}

static void DestroyHookItem(HookItem* item)
{
	if ( !item )
	{
		return;
	}

	MEMPOOL_FREE(item);
}

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

	HookItem* elementToDelete = NULL;
	HookItem* temp = NULL;

	DL_FOREACH_SAFE(item->list, elementToDelete, temp)
	{
		DL_DELETE(item->list, elementToDelete);
		DestroyHookItem(elementToDelete);
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

// TODO: Remove these guards once we call these functions in headless mode
#if !RAYGE_HEADLESS()
static bool
PassesModifierChecks(RayGE_InputSource source, const RayGE_InputBuffer* inputBuffer, unsigned int requiredModifiers)
{
	if ( source != INPUT_SOURCE_KEYBOARD )
	{
		// We don't support any other checks yet.
		return true;
	}

	if ( requiredModifiers == KEYMOD_NONE )
	{
		// No need to check anything.
		return true;
	}

	// Get the current modifiers.
	RayGE_KeyboardModifiers currentModifiers = InputBuffer_GetCurrentKeyboardModifiers(inputBuffer);

	// If we require that no modifiers are present at all, the check becomes simple:
	if ( requiredModifiers & KEYMOD_REQUIRE_NONE )
	{
		return currentModifiers == KEYMOD_NONE;
	}

	// Restrict the modifiers to only the ones we need to check.
	requiredModifiers &= KEYMOD_CHECK_MASK;
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

static void TriggerUIHooks(void)
{
	const bool uiIsOpen = UISubsystem_HasActiveMenus();

	// Deal with all newly inactive inputs before all newly active ones.
	for ( size_t source = 0; source < INPUT_SOURCE__COUNT; ++source )
	{
		const RayGE_InputSource inputSource = (RayGE_InputSource)source;

		const RayGE_InputBuffer* buffer = InputSubsystem_GetInputForSource(inputSource);
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

		const RayGE_InputBuffer* buffer = InputSubsystem_GetInputForSource(inputSource);
		RAYGE_ENSURE(buffer, "Expected valid input buffer for source");

		const CallbackInfo cbInfo = {
			inputSource,
			buffer,
			uiIsOpen,
		};

		InputBuffer_TriggerForAllInputsNowActive(buffer, &CheckAndCallHook, (void*)&cbInfo);
	}
}
#endif

void InputHookSubsystem_Init(void)
{
	if ( g_Data )
	{
		return;
	}

	g_Data = CreateData();
}

void InputHookSubsystem_ShutDown(void)
{
	if ( !g_Data )
	{
		return;
	}

	DestroyData(g_Data);
	g_Data = NULL;
}

void InputHookSubsystem_AddHook(RayGE_InputSource source, int id, unsigned int modifierFlags, RayGE_InputHook hook)
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

	HookItem* hookItem = CreateHookItem(modifierFlags, hook);
	DL_APPEND(hashItem->list, hookItem);
}

void InputHookSubsystem_RemoveAllHooksForInput(RayGE_InputSource source, int id)
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

	HookInputHashItem* hashItem = NULL;
	HASH_FIND_INT(g_Data->inputHash[source], &id, hashItem);

	if ( !hashItem )
	{
		return;
	}

	HookItem* item = NULL;
	HookItem* temp = NULL;

	DL_FOREACH_SAFE(hashItem->list, item, temp)
	{
		DL_DELETE(hashItem->list, item);
		DestroyHookItem(item);
	}
}

void InputHookSubsystem_ProcessInput(void)
{
	RAYGE_ASSERT_VALID(g_Data);

	if ( !g_Data )
	{
		return;
	}

#if !RAYGE_HEADLESS()
	TriggerUIHooks();
#endif
}
