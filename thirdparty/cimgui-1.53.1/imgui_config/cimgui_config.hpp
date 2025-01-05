#pragma once

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_IMPL_API extern "C"

#define IM_ASSERT(EXPR) cimgui_assert(EXPR, #EXPR, __FILE__, __LINE__, __func__)

extern "C" void cimgui_assert(bool expression, const char* description, const char* file, int line, const char* function);
