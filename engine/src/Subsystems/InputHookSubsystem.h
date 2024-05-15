#pragma once

#include "Subsystems/InputSubsystem.h"
#include "Input/InputBuffer.h"

typedef enum RayGE_InputHookTriggerFlag
{
	// This hook triggers when an input becomes active.
	INPUT_TRIGGER_ACTIVE = (1 << 0),

	// This hook triggers when an input becomes inactive.
	INPUT_TRIGGER_INACTIVE = (1 << 1),

	// This hook triggers even when a UI window is active.
	INPUT_TRIGGER_OVERRIDE_UI_FOCUS = (1 << 2),
} RayGE_InputHookTriggerFlag;

typedef void (*RayGE_InputHookCallback)(
	RayGE_InputSource source,
	int id,
	const RayGE_InputBuffer* inputBuffer,
	void* userData
);

typedef struct RayGE_InputHook
{
	unsigned int triggerFlags;
	RayGE_InputHookCallback callback;
	void* userData;
} RayGE_InputHook;

void InputHookSubsystem_Init(void);
void InputHookSubsystem_ShutDown(void);

void InputHookSubsystem_AddHook(RayGE_InputSource source, int id, unsigned int modifierFlags, RayGE_InputHook hook);

// Expected to be called *after* InputSubsystem.
void InputHookSubsystem_ProcessInput(void);
