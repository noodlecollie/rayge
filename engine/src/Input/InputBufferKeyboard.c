#include "Input/InputBufferKeyboard.h"
#include "Subsystems/LoggingSubsystem.h"
#include "raylib.h"

static RayGE_KeyboardModifiers GetKeyboardModifiers(const int* bufferData, size_t length)
{
	RayGE_KeyboardModifiers modifiers = KEYMOD_NONE;

	for ( size_t index = 0; index < length; ++index )
	{
		int key = bufferData[index];

		if ( key == KEY_LEFT_CONTROL || key == KEY_RIGHT_CONTROL )
		{
			modifiers |= KEYMOD_CTRL;
		}
		else if ( key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT )
		{
			modifiers |= KEYMOD_SHIFT;
		}
		else if ( key == KEY_LEFT_ALT || key == KEY_RIGHT_ALT )
		{
			modifiers |= KEYMOD_ALT;
		}
	}

	return modifiers;
}

void InputBuffer_PopulateFromKeyboard(RayGE_InputBuffer* buffer)
{
	if ( !buffer )
	{
		return;
	}

	InputBuffer_Swap(buffer);
	InputBuffer_SetCurrentBufferValues(buffer, KEY_NULL);

	int* bufferData = InputBuffer_GetCurrentBuffer(buffer);
	const size_t bufferMaxLength = InputBuffer_GetMaxLength(buffer);
	size_t nextIndex = 0;

	for ( int pressedKey = GetKeyPressed(); pressedKey != KEY_NULL; pressedKey = GetKeyPressed() )
	{
		if ( nextIndex >= bufferMaxLength )
		{
			LoggingSubsystem_PrintLine(
				RAYGE_LOG_WARNING,
				"Key %d exceeded max of %zu simultaneous key presses per frame",
				pressedKey,
				bufferMaxLength
			);

			continue;
		}

		bufferData[nextIndex++] = pressedKey;
	}
}

RayGE_KeyboardModifiers InputBuffer_GetCurrentKeyboardModifiers(RayGE_InputBuffer* buffer)
{
	if ( !buffer )
	{
		return KEYMOD_NONE;
	}

	return GetKeyboardModifiers(InputBuffer_GetCurrentBuffer(buffer), InputBuffer_GetMaxLength(buffer));
}

RayGE_KeyboardModifiers InputBuffer_GetKeyboardModifiersPressed(RayGE_InputBuffer* buffer)
{
	if ( !buffer )
	{
		return KEYMOD_NONE;
	}

	RayGE_KeyboardModifiers oldModifiers =
		GetKeyboardModifiers(InputBuffer_GetLastBuffer(buffer), InputBuffer_GetMaxLength(buffer));

	RayGE_KeyboardModifiers newModifiers =
		GetKeyboardModifiers(InputBuffer_GetCurrentBuffer(buffer), InputBuffer_GetMaxLength(buffer));

	return newModifiers & (~oldModifiers);
}

RayGE_KeyboardModifiers InputBuffer_GetKeyboardModifiersReleased(RayGE_InputBuffer* buffer)
{
	if ( !buffer )
	{
		return KEYMOD_NONE;
	}

	RayGE_KeyboardModifiers oldModifiers =
		GetKeyboardModifiers(InputBuffer_GetLastBuffer(buffer), InputBuffer_GetMaxLength(buffer));

	RayGE_KeyboardModifiers newModifiers =
		GetKeyboardModifiers(InputBuffer_GetCurrentBuffer(buffer), InputBuffer_GetMaxLength(buffer));

	return oldModifiers & (~newModifiers);
}
