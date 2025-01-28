// Taken from https://github.com/raylib-extras/rlImGui/blob/main/imgui_impl_raylib.h,
// but we have to roll all this ourselves because we need full control.

#pragma once

#include <stdbool.h>
#include "cimgui.h"

void ImGui_ImplRaylib_Init(void);
void ImGui_ImplRaylib_BuildFontAtlas(void);
void ImGui_ImplRaylib_Shutdown(void);
void ImGui_ImplRaylib_NewFrame(void);
void ImGui_ImplRaylib_ProcessEvents(void);
void ImGui_ImplRaylib_Render(void);
