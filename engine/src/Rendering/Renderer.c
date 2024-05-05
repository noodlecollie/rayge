#include "Rendering/Renderer.h"
#include "Subsystems/RendererSubsystem.h"

void Renderer_DrawTextDev(const char* text, int posX, int posY, Color color)
{
	DrawTextEx(
		RenderSubsystem_GetDefaultFont(),
		text,
		(Vector2) {(float)posX, (float)posY},
		RENDERSUBSYSTEM_DEFAULT_FONT_SIZE,
		1.0f,
		color
	);
}
