#pragma once

typedef unsigned int RayGE_KeyboardModifiers;

typedef enum RayGE_KeyboardModifierFlag
{
	KEYMOD_NONE = 0,
	KEYMOD_CTRL = (1 << 0),
	KEYMOD_SHIFT = (1 << 1),
	KEYMOD_ALT = (1 << 2),
} RayGE_KeyboardModifierFlag;
