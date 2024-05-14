#pragma once

#include "Subsystems/InputSubsystem.h"
#include "Input/InputBuffer.h"

typedef enum RayGE_InputHookTriggerFlag
{
	INPUT_TRIGGER_ACTIVE = (1 << 0),
	INPUT_TRIGGER_INACTIVE = (1 << 1)
} RayGE_InputHookTriggerFlag;

typedef struct RayGE_InputHook
{
	unsigned int triggerFlags;
	void (*callback)(RayGE_InputSource source, int id, const RayGE_InputBuffer* inputBuffer, void* userData);
	void* userData;
} RayGE_InputHook;

void InputHookSubsystem_Init(void);
void InputHookSubsystem_ShutDown(void);

void InputHookSubsystem_AddHook(RayGE_InputSource source, int id, RayGE_InputHook hook);

// Expected to be called *after* InputSubsystem.
void InputHookSubsystem_ProcessInput(void);
