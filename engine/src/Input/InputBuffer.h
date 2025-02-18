#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "wzl_cutl/attributes.h"

typedef struct RayGE_InputBuffer RayGE_InputBuffer;

typedef enum RayGE_InputState
{
	INPUT_STATE_INACTIVE = 0,
	INPUT_STATE_ACTIVE
} RayGE_InputState;

typedef void (*RayGE_InputBufferTriggerFunc)(int id, RayGE_InputState state, void* userData);

WZL_ATTR_NODISCARD RayGE_InputBuffer* InputBuffer_Create(size_t maxSimultaneousInputs);
void InputBuffer_Destroy(RayGE_InputBuffer* buffer);

size_t InputBuffer_GetMaxLength(const RayGE_InputBuffer* buffer);
int* InputBuffer_GetCurrentBuffer(RayGE_InputBuffer* buffer);
const int* InputBuffer_GetCurrentBufferConst(const RayGE_InputBuffer* buffer);
int* InputBuffer_GetLastBuffer(RayGE_InputBuffer* buffer);
const int* InputBuffer_GetLastBufferConst(const RayGE_InputBuffer* buffer);
int* InputBuffer_GetUnicodeCharBuffer(RayGE_InputBuffer* buffer);
const int* InputBuffer_GetUnicodeCharBufferConst(const RayGE_InputBuffer* buffer);
void InputBuffer_SetCurrentBufferValues(RayGE_InputBuffer* buffer, int value);
void InputBuffer_SetLastBufferValues(RayGE_InputBuffer* buffer, int value);
void InputBuffer_ClearUnicodeBufferValues(RayGE_InputBuffer* buffer);
void InputBuffer_Swap(RayGE_InputBuffer* buffer);

// Returns true if the action has occurred on this poll.
bool InputBuffer_InputIsNowActive(const RayGE_InputBuffer* buffer, int value);
bool InputBuffer_InputIsNowInctive(const RayGE_InputBuffer* buffer, int value);

// Returns true if the key is currently active, regardless of the previous state.
bool InputBuffer_InputIsCurrentlyActive(const RayGE_InputBuffer* buffer, int value);

// Returns true if the key was active on the last poll, regardless of the current state.
bool InputBuffer_InputWasActive(const RayGE_InputBuffer* buffer, int value);

void InputBuffer_TriggerForAllInputsNowActive(
	const RayGE_InputBuffer* buffer,
	RayGE_InputBufferTriggerFunc callback,
	void* userData
);

void InputBuffer_TriggerForAllInputsNowInactive(
	const RayGE_InputBuffer* buffer,
	RayGE_InputBufferTriggerFunc callback,
	void* userData
);
