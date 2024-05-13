// This file is adapted from raylib-nuklear. See the license below.

/**********************************************************************************************
*
*   raylib-nuklear v4.5.1 - Nuklear GUI for Raylib.
*
*   FEATURES:
*       - Use the Nuklear immediate-mode graphical user interface in raylib.
*
*   DEPENDENCIES:
*       - raylib 4.2+ https://www.raylib.com/
*       - Nuklear https://github.com/Immediate-Mode-UI/Nuklear
*
*   LICENSE: zlib/libpng
*
*   raylib-nuklear is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   Copyright (c) 2020 Rob Loach (@RobLoach)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#pragma once

// Nuklear config:
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_KEYSTATE_BASED_INPUT
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define RAYLIB_NUKLEAR_DEFAULT_FONTSIZE 20

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4116)
#endif

#include "nuklear-4.12.0/nuklear.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

NK_API struct nk_context* InitNuklear(int fontSize);                // Initialize the Nuklear GUI context
NK_API struct nk_context* InitNuklearEx(Font font, float fontSize); // Initialize the Nuklear GUI context, with a custom font
NK_API void UpdateNuklear(struct nk_context * ctx);                 // Update the input state and internal components for Nuklear
NK_API void DrawNuklear(struct nk_context * ctx);                   // Render the Nuklear GUI on the screen
NK_API void UnloadNuklear(struct nk_context * ctx);                 // Deinitialize the Nuklear context
NK_API struct nk_color ColorToNuklear(Color color);                 // Convert a raylib Color to a Nuklear color object
NK_API struct nk_colorf ColorToNuklearF(Color color);               // Convert a raylib Color to a Nuklear floating color
NK_API struct Color ColorFromNuklear(struct nk_color color);        // Convert a Nuklear color to a raylib Color
NK_API struct Color ColorFromNuklearF(struct nk_colorf color);      // Convert a Nuklear floating color to a raylib Color
NK_API struct Rectangle RectangleFromNuklear(struct nk_context * ctx, struct nk_rect rect); // Convert a Nuklear rectangle to a raylib Rectangle
NK_API struct nk_rect RectangleToNuklear(struct nk_context * ctx, Rectangle rect); // Convert a raylib Rectangle to a Nuklear Rectangle
NK_API struct nk_image TextureToNuklear(Texture tex);               // Convert a raylib Texture to A Nuklear image
NK_API struct Texture TextureFromNuklear(struct nk_image img);      // Convert a Nuklear image to a raylib Texture
NK_API struct nk_image LoadNuklearImage(const char* path);          // Load a Nuklear image
NK_API void UnloadNuklearImage(struct nk_image img);                // Unload a Nuklear image. And free its data
NK_API void CleanupNuklearImage(struct nk_image img);               // Frees the data stored by the Nuklear image
NK_API void SetNuklearScaling(struct nk_context * ctx, float scaling); // Sets the scaling for the given Nuklear context
NK_API float GetNuklearScaling(struct nk_context * ctx);            // Retrieves the scaling of the given Nuklear context

#ifdef __cplusplus
}
#endif
