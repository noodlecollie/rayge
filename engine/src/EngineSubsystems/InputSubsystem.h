#pragma once

#include "Input/InputBuffer.h"

typedef enum RayGE_InputSource
{
	INPUT_SOURCE_KEYBOARD = 0,

	INPUT_SOURCE__COUNT
} RayGE_InputSource;

#define RAYGE_INPUT_LAYER_LIST \
	LIST_ITEM(INPUT_LAYER_GAME, "Game") \
	LIST_ITEM(INPUT_LAYER_UI, "UI")

typedef enum RayGE_InputLayer
{
#define LIST_ITEM(enum, name) enum,
	RAYGE_INPUT_LAYER_LIST
#undef LIST_ITEM

		INPUT_LAYER__COUNT
} RayGE_InputLayer;

void InputSubsystem_Init(void);
void InputSubsystem_ShutDown(void);

void InputSubsystem_ClearAllInputThisFrame(void);

const RayGE_InputBuffer* InputSubsystem_GetInputBuffer(RayGE_InputSource source, RayGE_InputLayer layer);

RayGE_InputLayer InputSubsystem_GetCurrentInputLayer(void);
void InputSubsystem_SetCurrentInputLayer(RayGE_InputLayer layer);

void InputSubsystem_NewFrame(void);
void InputSubsystem_ProcessInput(void);
