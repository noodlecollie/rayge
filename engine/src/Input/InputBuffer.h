#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef struct RayGE_InputBuffer RayGE_InputBuffer;

typedef enum RayGE_InputState
{
	INPUT_STATE_INACTIVE = 0,
	INPUT_STATE_ACTIVE
} RayGE_InputState;

typedef void (*RayGE_InputBufferTriggerFunc)(int id, RayGE_InputState state, void* userData);

RayGE_InputBuffer* InputBuffer_Create(size_t maxSimultaneousInputs);
void InputBuffer_Destroy(RayGE_InputBuffer* buffer);

size_t InputBuffer_GetMaxLength(const RayGE_InputBuffer* buffer);
int* InputBuffer_GetCurrentBuffer(RayGE_InputBuffer* buffer);
const int* InputBuffer_GetCurrentBufferConst(const RayGE_InputBuffer* buffer);
int* InputBuffer_GetLastBuffer(RayGE_InputBuffer* buffer);
const int* InputBuffer_GetLastBufferConst(const RayGE_InputBuffer* buffer);
void InputBuffer_SetCurrentBufferValues(RayGE_InputBuffer* buffer, int value);
void InputBuffer_SetLastBufferValues(RayGE_InputBuffer* buffer, int value);
void InputBuffer_Swap(RayGE_InputBuffer* buffer);

bool InputBuffer_InputIsNowActive(const RayGE_InputBuffer* buffer, int value);
bool InputBuffer_InputIsNowInctive(const RayGE_InputBuffer* buffer, int value);

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
