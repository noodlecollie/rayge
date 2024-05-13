#pragma once

#include <stddef.h>

typedef struct RayGE_InputBuffer RayGE_InputBuffer;

RayGE_InputBuffer* InputBuffer_Create(size_t maxSimultaneousInputs);
void InputBuffer_Destroy(RayGE_InputBuffer* buffer);

size_t InputBuffer_GetMaxLength(const RayGE_InputBuffer* buffer);
int* InputBuffer_GetCurrentBuffer(RayGE_InputBuffer* buffer);
int* InputBuffer_GetLastBuffer(RayGE_InputBuffer* buffer);
void InputBuffer_SetCurrentBufferValues(RayGE_InputBuffer* buffer, int value);
void InputBuffer_SetLastBufferValues(RayGE_InputBuffer* buffer, int value);
void InputBuffer_Swap(RayGE_InputBuffer* buffer);
