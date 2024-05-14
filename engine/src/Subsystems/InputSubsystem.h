#pragma once

#include "Input/InputBuffer.h"

void InputSubsystem_Init(void);
void InputSubsystem_ShutDown(void);

void InputSubsystem_ClearAllInputThisFrame(void);

const RayGE_InputBuffer* InputSubsystem_GetKeyboardBuffer(void);

void InputSubsystem_NewFrame(void);
void InputSubsystem_ProcessInput(void);
