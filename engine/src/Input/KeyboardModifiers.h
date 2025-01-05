#pragma once

typedef unsigned int RayGE_KeyboardModifiers;

typedef enum RayGE_KeyboardModifierFlag
{
	// Completely ignore modifier key state when deciding
	// whether to activate.
	KEYMOD_NONE = 0,

	KEYMOD_CTRL = (1 << 0),
	KEYMOD_SHIFT = (1 << 1),
	KEYMOD_ALT = (1 << 2),
	KEYMOD_CHECK_MASK = KEYMOD_CTRL | KEYMOD_SHIFT | KEYMOD_ALT,

	// Require that no modifier keys are pressed in order to activate.
	KEYMOD_REQUIRE_NONE = (1 << 3),
} RayGE_KeyboardModifierFlag;
