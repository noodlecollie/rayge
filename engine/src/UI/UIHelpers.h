#pragma once

#include "Nuklear/Nuklear.h"

#define UI_DEFAULT_WINDOW_FLAGS \
	(NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_CLOSABLE)

#define UI_DEFAULT_GROUP_FLAGS (NK_PANEL_GROUP | NK_WINDOW_TITLE | NK_WINDOW_BORDER)
#define UI_DEFAULT_ROW_HEIGHT 20.0f

// For keeping track of window sizes. Returns the current rect if it has
// a valid width and height, otherwise returns the default rect.
// The current rect is intended to represent the last size of the window,
// so if it is not valid, the window should be sized according to the default.
static inline struct nk_rect UI_CalcWindowDims(struct nk_rect currentDim, struct nk_rect defaultDim)
{
	return (currentDim.w > 0 && currentDim.h > 0) ? currentDim : defaultDim;
}
