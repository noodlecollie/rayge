#pragma once

#include <stdbool.h>
#include "raylib.h"

typedef struct RayGE_InputCommand
{
	KeyboardKey key;
	void (*PressFunction)(void);
	void (*ReleaseFunction)(void);
} RayGE_InputCommand;

void InputSubsystem_Init(void);
void InputSubsystem_ShutDown(void);

void InputSubsystem_RegisterCommand(RayGE_InputCommand command);
void InputSubsystem_ExecuteCommand(KeyboardKey key, bool pressed);
