#pragma once

#include "RayGE/Math.h"

void GameWindow_SetDimensions(Vector2i dim);
void GameWindow_SetTitle(const char* title);
void GameWindow_Create(void);
void GameWindow_Destroy(void);

// The close request usually comes from the underlying platform,
// eg. if someone has pressed the close button.
bool GameWindow_CloseRequested(void);
