#pragma once

#include <stdbool.h>
#include "raylib.h"
#include "Non-Headless/Rendering/Renderer.h"

#define RENDERERMODULE_DEFAULT_FONT_SIZE 20

void RendererSubsystem_Init(void);
void RendererSubsystem_ShutDown(void);
bool RendererSubsystem_IsInitialised(void);

// The close request usually comes from the underlying platform,
// eg. if someone has pressed the close button.
bool RendererSubsystem_IsWindowCloseRequested(void);

RayGE_Renderer* RendererSubsystem_GetRenderer(void);
Font RendererSubsystem_GetDefaultMonoFont(void);
Font RendererSubsystem_GetDefaultUIFont(void);
