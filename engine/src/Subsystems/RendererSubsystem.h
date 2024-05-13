#pragma once

#include <stdbool.h>
#include "raylib.h"

#define RENDERSUBSYSTEM_DEFAULT_FONT_SIZE 20

void RendererSubsystem_Init(void);
void RendererSubsystem_ShutDown(void);
bool RendererSubsystem_IsInitialised(void);

// The close request usually comes from the underlying platform,
// eg. if someone has pressed the close button.
bool RendererSubsystem_WindowCloseRequested(void);

Font RendererSubsystem_GetDefaultMonoFont(void);
Font RendererSubsystem_GetDefaultUIFont(void);
