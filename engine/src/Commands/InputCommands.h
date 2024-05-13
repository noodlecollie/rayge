#pragma once

#include "raylib.h"

typedef struct RayGE_InputCommand
{
	KeyboardKey key;
	void (*PressFunction)(void);
	void (*ReleaseFunction)(void);
};

void InputCommand_Register(RayGE_InputCommand command);
