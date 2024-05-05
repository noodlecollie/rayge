#include <stdarg.h>
#include "Rendering/Renderer.h"
#include "Subsystems/RendererSubsystem.h"
#include "Scene/Entity.h"
#include "Debugging.h"

#define DBG_LOCATION_MARKER_RADIUS 4.0f
#define MAX_DEV_TEXT_LENGTH 256

static uint64_t g_DebugFlags = 0;

static void DrawEntityLocation(RayGE_Entity* entity)
{
	RayGE_ComponentHeader* cmpHeader = Entity_GetFirstComponentOfType(entity, RAYGE_COMPONENTTYPE_SPATIAL);

	if ( !cmpHeader )
	{
		// No spatial information.
		return;
	}

	RayGE_ComponentImpl_Spatial* component = COMPONENTCAST_SPATIAL(cmpHeader, true);

	Vector3 forward;
	Vector3 right;
	EulerAnglesToBasis(component->data.angles, &forward, &right, NULL);

	// Yaw circle
	DrawCircle3D(component->data.position, DBG_LOCATION_MARKER_RADIUS, (Vector3) {0.0f, 0.0f, 1.0f}, 0.0f, GREEN);

	// TODO: Need pitch and roll indicators here, but need to sort out how Euler angles relate
	// to raylib math operations first. We may be able to remove our custom implementation.

	DrawLine3D(
		component->data.position,
		Vector3Add(component->data.position, Vector3Scale(forward, DBG_LOCATION_MARKER_RADIUS * 1.1f)),
		YELLOW
	);
}

void Renderer_AddDebugFlags(uint64_t flags)
{
	g_DebugFlags |= flags;
}

void Renderer_RemoveDebugFlags(uint64_t flags)
{
	g_DebugFlags &= ~flags;
}

void Renderer_ClearDebugFlags(void)
{
	g_DebugFlags = 0;
}

void Renderer_DrawTextDev(int posX, int posY, Color color, const char* text)
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

void Renderer_FormatTextDev(int posX, int posY, Color color, const char* format, ...)
{
	char buffer[MAX_DEV_TEXT_LENGTH];

	va_list args;
	va_start(args, format);
	wzl_vsprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	Renderer_DrawTextDev(posX, posY, color, buffer);
}

void Renderer_DrawEntity(RayGE_Entity* entity)
{
	RAYGE_ASSERT(RendererSubsystem_IsInitialised(), "Renderer subsystem must be initialised");

	if ( !entity )
	{
		return;
	}

	if ( g_DebugFlags & RENDERER_DBG_DRAW_LOCATIONS )
	{
		DrawEntityLocation(entity);
	}
}
