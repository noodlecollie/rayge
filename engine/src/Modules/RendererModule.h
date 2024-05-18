#pragma once

#include <stdbool.h>
#include "raylib.h"
#include "Rendering/Renderer.h"

#define RENDERERMODULE_DEFAULT_FONT_SIZE 20

void RendererModule_Init(void);
void RendererModule_ShutDown(void);
bool RendererModule_IsInitialised(void);

// The close request usually comes from the underlying platform,
// eg. if someone has pressed the close button.
bool RendererModule_WindowCloseRequested(void);

RayGE_Renderer* RendererModule_GetRenderer(void);
Font RendererModule_GetDefaultMonoFont(void);
Font RendererModule_GetDefaultUIFont(void);
