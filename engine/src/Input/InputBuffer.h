#pragma once

#include <stddef.h>

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
int* InputBuffer_GetLastBuffer(RayGE_InputBuffer* buffer);
void InputBuffer_SetCurrentBufferValues(RayGE_InputBuffer* buffer, int value);
void InputBuffer_SetLastBufferValues(RayGE_InputBuffer* buffer, int value);
void InputBuffer_Swap(RayGE_InputBuffer* buffer);

void InputBuffer_TriggerForAllInputsNowActive(
	RayGE_InputBuffer* buffer,
	RayGE_InputBufferTriggerFunc callback,
	void* userData
);

void InputBuffer_TriggerForAllInputsNowInactive(
	RayGE_InputBuffer* buffer,
	RayGE_InputBufferTriggerFunc callback,
	void* userData
);
