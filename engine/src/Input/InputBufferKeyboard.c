#include "Input/InputBufferKeyboard.h"
#include "Subsystems/LoggingSubsystem.h"
#include "raylib.h"

static bool IsKeyInList(int key, const int* list, size_t length)
{
	for ( size_t index = 0; index < length; ++index )
	{
		if ( list[index] == key )
		{
			return true;
		}
	}

	return false;
}

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

	InputBuffer_SetCurrentBufferValues(buffer, KEY_NULL);

	const int* lastData = InputBuffer_GetLastBufferConst(buffer);
	int* bufferData = InputBuffer_GetCurrentBuffer(buffer);
	const size_t bufferMaxLength = InputBuffer_GetMaxLength(buffer);
	size_t nextIndex = 0;

	// Firstly, check each key that was pressed last time and see if it's still pressed.
	// GetKeyPressed() only reports new presses, not existing ones.
	for ( size_t index = 0; index < bufferMaxLength && lastData[index] != KEY_NULL; ++index )
	{
		if ( !IsKeyDown(lastData[index]) )
		{
			continue;
		}

		if ( nextIndex >= bufferMaxLength )
		{
			LoggingSubsystem_PrintLine(
				RAYGE_LOG_WARNING,
				"Key %d exceeded max of %zu simultaneous key presses per frame",
				lastData[index],
				bufferMaxLength
			);

			continue;
		}

		bufferData[nextIndex++] = lastData[index];
	}

	// Now see if any new keys are pressed.
	for ( int pressedKey = GetKeyPressed(); pressedKey != KEY_NULL; pressedKey = GetKeyPressed() )
	{
		// Don't re-record this key if for some reason we already saw it.
		if ( IsKeyInList(pressedKey, bufferData, nextIndex) )
		{
			continue;
		}

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

RayGE_KeyboardModifiers InputBuffer_GetCurrentKeyboardModifiers(const RayGE_InputBuffer* buffer)
{
	if ( !buffer )
	{
		return KEYMOD_NONE;
	}

	return GetKeyboardModifiers(InputBuffer_GetCurrentBufferConst(buffer), InputBuffer_GetMaxLength(buffer));
}

RayGE_KeyboardModifiers InputBuffer_GetKeyboardModifiersPressed(const RayGE_InputBuffer* buffer)
{
	if ( !buffer )
	{
		return KEYMOD_NONE;
	}

	RayGE_KeyboardModifiers oldModifiers =
		GetKeyboardModifiers(InputBuffer_GetLastBufferConst(buffer), InputBuffer_GetMaxLength(buffer));

	RayGE_KeyboardModifiers newModifiers =
		GetKeyboardModifiers(InputBuffer_GetCurrentBufferConst(buffer), InputBuffer_GetMaxLength(buffer));

	return newModifiers & (~oldModifiers);
}

RayGE_KeyboardModifiers InputBuffer_GetKeyboardModifiersReleased(const RayGE_InputBuffer* buffer)
{
	if ( !buffer )
	{
		return KEYMOD_NONE;
	}

	RayGE_KeyboardModifiers oldModifiers =
		GetKeyboardModifiers(InputBuffer_GetLastBufferConst(buffer), InputBuffer_GetMaxLength(buffer));

	RayGE_KeyboardModifiers newModifiers =
		GetKeyboardModifiers(InputBuffer_GetCurrentBufferConst(buffer), InputBuffer_GetMaxLength(buffer));

	return oldModifiers & (~newModifiers);
}
