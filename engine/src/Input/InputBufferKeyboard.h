#pragma once

#include "Input/InputBuffer.h"
#include "Input/KeyboardModifiers.h"

void InputBuffer_PopulateFromKeyboard(RayGE_InputBuffer* buffer);
RayGE_KeyboardModifiers InputBuffer_GetCurrentKeyboardModifiers(RayGE_InputBuffer* buffer);
RayGE_KeyboardModifiers InputBuffer_GetKeyboardModifiersPressed(RayGE_InputBuffer* buffer);
RayGE_KeyboardModifiers InputBuffer_GetKeyboardModifiersReleased(RayGE_InputBuffer* buffer);
