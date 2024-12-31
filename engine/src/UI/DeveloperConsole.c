#include <stddef.h>
#include <string.h>
#include <limits.h>
#include "UI/DeveloperConsole.h"
#include "MemPool/MemPoolManager.h"
#include "UI/UIHelpers.h"
#include "Logging/Logging.h"
#include "Nuklear/Nuklear.h"
#include "raylib.h"
#include "wzl_cutl/math.h"

typedef struct WindowState
{
	Vector2 windowPos;
	Vector2 windowDim;

	char* logBuffer;
	size_t logBufferSize;
	size_t logBufferStringLength;
	size_t logCounter;
} WindowState;

static WindowState g_WindowState;

static Vector2 GetSensibleWindowPosition(const WindowState* state)
{
	Vector2 pos = state->windowPos;
	const float winWidth = (float)GetRenderWidth();
	const float winHeight = (float)GetRenderHeight();

	if ( state->windowPos.x + state->windowDim.x >= winWidth - 1.0f )
	{
		pos.x = winWidth - state->windowDim.x - 1;
	}

	if ( pos.x < 0.0f )
	{
		pos.x = 0.0f;
	}

	if ( state->windowPos.y + state->windowDim.y >= winHeight - 1.0f )
	{
		pos.y = winHeight - state->windowDim.y - 1;
	}

	if ( pos.y < 0.0f )
	{
		pos.y = 0.0f;
	}

	return pos;
}

static Vector2 GetSensibleWindowDimensions(const WindowState* state)
{
	Vector2 dim = state->windowDim;

	if ( dim.x < 1.0f )
	{
		dim.x = (float)GetRenderWidth() * 0.5f;
	}

	if ( dim.y < 1.0f )
	{
		dim.y = (float)GetRenderHeight() * 0.75f;
	}

	return dim;
}

static void Show(struct nk_context* context, void* userData)
{
	(void)context;

	WindowState* state = (WindowState*)userData;

	if ( !state->logBuffer )
	{
		size_t size = Logging_GetLogBufferMaxMessageLength();

		if ( size > 0 )
		{
			state->logBufferSize = size;
			state->logBuffer = MEMPOOL_MALLOC(MEMPOOL_UI, state->logBufferSize);
			state->logBufferStringLength = 0;
		}
	}
}

static void Hide(struct nk_context* context, void* userData)
{
	(void)context;

	WindowState* state = (WindowState*)userData;

	if ( state->logBuffer )
	{
		MEMPOOL_FREE(state->logBuffer);
		state->logBuffer = NULL;
		state->logBufferSize = 0;
		state->logBufferStringLength = 0;
	}

	state->logCounter = 0;
}

static bool Poll(struct nk_context* context, void* userData)
{
	WindowState* state = (WindowState*)userData;

	bool shouldStayOpen = false;
	const Vector2 pos = GetSensibleWindowPosition(state);
	const Vector2 dim = GetSensibleWindowDimensions(state);
	size_t logCounter = Logging_GetLogCounter();

	if ( state->logCounter != logCounter )
	{
		state->logCounter = logCounter;

		if ( state->logBuffer )
		{
			const char* message = Logging_GetLogBufferBase();
			state->logBufferStringLength = Logging_GetLogBufferTotalMessageLength();
			state->logBufferStringLength = WZL_MIN(state->logBufferStringLength, state->logBufferSize - 1);

			memcpy(state->logBuffer, message, state->logBufferStringLength - 1);
			state->logBuffer[state->logBufferStringLength] = '\0';
		}
	}

	if ( nk_begin_titled(
			 context,
			 "developer_console",
			 "Developer Console",
			 (struct nk_rect) {pos.x, pos.y, dim.x, dim.y},
			 UI_DEFAULT_WINDOW_FLAGS
		 ) )
	{
		nk_layout_row_dynamic(context, dim.y - 100, 1);

		if ( state->logBuffer )
		{
			// TODO: Draw the text box. Nuklear doesn't seem to be very good for this...
			// int length = (int)WZL_MIN(state->logBufferStringLength, (size_t)INT_MAX);
		}

		struct nk_vec2 winPos = nk_window_get_position(context);
		state->windowPos.x = winPos.x;
		state->windowPos.y = winPos.y;

		struct nk_vec2 winDim = nk_window_get_size(context);
		state->windowDim.x = winDim.x;
		state->windowDim.y = winDim.y;

		shouldStayOpen = true;
	}

	nk_end(context);
	return shouldStayOpen;
}

const RayGE_UIMenu Menu_DeveloperConsole = {
	&g_WindowState,

	Show,
	Hide,
	Poll,
};
