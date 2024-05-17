#pragma once

#include "Input/InputBuffer.h"

typedef enum RayGE_InputSource
{
	INPUT_SOURCE_KEYBOARD = 0,

	INPUT_SOURCE__COUNT
} RayGE_InputSource;

void InputModule_Init(void);
void InputModule_ShutDown(void);

void InputModule_ClearAllInputThisFrame(void);

const RayGE_InputBuffer* InputModule_GetInputForSource(RayGE_InputSource source);

void InputModule_NewFrame(void);
void InputModule_ProcessInput(void);
