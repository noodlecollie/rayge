#pragma once

#include "Input/InputBuffer.h"
#include "Input/KeyboardModifiers.h"

void InputBuffer_PopulateFromKeyboard(RayGE_InputBuffer* buffer);
RayGE_KeyboardModifiers InputBuffer_GetCurrentKeyboardModifiers(const RayGE_InputBuffer* buffer);
RayGE_KeyboardModifiers InputBuffer_GetKeyboardModifiersPressed(const RayGE_InputBuffer* buffer);
RayGE_KeyboardModifiers InputBuffer_GetKeyboardModifiersReleased(const RayGE_InputBuffer* buffer);
