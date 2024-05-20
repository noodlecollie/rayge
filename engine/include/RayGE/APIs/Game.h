// File: Game.h
// Interface for general game-related functions and callbacks.

#pragma once

#include "RayGE/Math.h"

typedef struct RayGE_Game_Callbacks
{
	void (*StartUp)(void);
	void (*ShutDown)(void);
} RayGE_Game_Callbacks;
