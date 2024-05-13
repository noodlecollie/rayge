#pragma once

#include <stdbool.h>
#include "raylib.h"

typedef enum RayGE_InputType
{
	INPUT_TYPE_KEYBOARD = 0,

	INPUT_TYPE__COUNT
} RayGE_InputType;

typedef struct RayGE_InputCommand
{
	RayGE_InputType type;
	int id;
	void (*PressFunction)(void);
	void (*ReleaseFunction)(void);
} RayGE_InputCommand;

void InputSubsystem_Init(void);
void InputSubsystem_ShutDown(void);

void InputSubsystem_RegisterCommand(RayGE_InputCommand command);
void InputSubsystem_ProcessInput(void);
void InputSubsystem_ReleaseAllKeys(void);
