#pragma once

#include "Input/InputBuffer.h"

typedef enum RayGE_InputSource
{
	INPUT_SOURCE_KEYBOARD = 0,

	INPUT_SOURCE__COUNT
} RayGE_InputSource;

void InputSubsystem_Init(void);
void InputSubsystem_ShutDown(void);

void InputSubsystem_ClearAllInputThisFrame(void);

const RayGE_InputBuffer* InputSubsystem_GetInputForSource(RayGE_InputSource source);

void InputSubsystem_NewFrame(void);
void InputSubsystem_ProcessInput(void);
