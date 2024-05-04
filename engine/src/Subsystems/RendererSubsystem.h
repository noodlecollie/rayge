#pragma once

#include <stdbool.h>
#include "raylib.h"

void RendererSubsystem_Init(void);
void RendererSubsystem_ShutDown(void);

// The close request usually comes from the underlying platform,
// eg. if someone has pressed the close button.
bool RenderSubsystem_WindowCloseRequested(void);

// Prints some text to the screen for quick development purposes.
void RendererSubsystem_DrawTextDev(const char *text, int posX, int posY, Color color);
