#include <stdarg.h>
#include "Rendering/Renderer.h"
#include "Modules/RendererModule.h"
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

	// World X
	DrawLine3D(
		Vector3Add(component->data.position, (Vector3) {-0.2f * DBG_LOCATION_MARKER_RADIUS, 0.0f, 0.0f}),
		Vector3Add(component->data.position, (Vector3) {1.2f * DBG_LOCATION_MARKER_RADIUS, 0.0f, 0.0f}),
		RED
	);

	// World Y
	DrawLine3D(
		Vector3Add(component->data.position, (Vector3) {0.0f, -0.2f * DBG_LOCATION_MARKER_RADIUS, 0.0f}),
		Vector3Add(component->data.position, (Vector3) {0.0f, 1.2f * DBG_LOCATION_MARKER_RADIUS, 0.0f}),
		GREEN
	);

	// World Z
	DrawLine3D(
		Vector3Add(component->data.position, (Vector3) {0.0f, 0.0f, -0.2f * DBG_LOCATION_MARKER_RADIUS}),
		Vector3Add(component->data.position, (Vector3) {0.0f, 0.0f, 1.2f * DBG_LOCATION_MARKER_RADIUS}),
		BLUE
	);

	// Yaw circle
	DrawCircle3D(component->data.position, DBG_LOCATION_MARKER_RADIUS, (Vector3) {0.0f, 0.0f, 1.0f}, 0.0f, GREEN);

	// Rotate pitch circle from being an XY disc to an XZ disc.
	Quaternion pitchRot =
		QuaternionFromAxisAngle((Vector3) {1.0f, 0.0f, 0.0}, DEG2RAD * (90.0f + component->data.angles.roll));

	// Rotate to point in direction of yaw.
	pitchRot = QuaternionMultiply(
		QuaternionFromAxisAngle((Vector3) {0.0f, 0.0f, 1.0f}, DEG2RAD * component->data.angles.yaw),
		pitchRot
	);

	Vector3 rotAxis;
	float rotAngle;
	QuaternionToAxisAngle(pitchRot, &rotAxis, &rotAngle);

	// Pitch circle
	DrawCircle3D(component->data.position, DBG_LOCATION_MARKER_RADIUS, rotAxis, RAD2DEG * rotAngle, RED);

	// Rotate roll circle from being an XY disc to an XZ disc.
	Quaternion rollRot = QuaternionFromAxisAngle((Vector3) {1.0f, 0.0f, 0.0}, DEG2RAD * 90.0f);

	// Rotate to point perpendicular to yaw.
	rollRot = QuaternionMultiply(
		QuaternionFromAxisAngle((Vector3) {0.0f, 0.0f, 1.0f}, DEG2RAD * (component->data.angles.yaw - 90.0f)),
		rollRot
	);

	QuaternionToAxisAngle(rollRot, &rotAxis, &rotAngle);

	// Roll circle
	DrawCircle3D(component->data.position, DBG_LOCATION_MARKER_RADIUS, rotAxis, RAD2DEG * rotAngle, BLUE);

	Vector3 endPoint = Vector3Add(component->data.position, Vector3Scale(forward, DBG_LOCATION_MARKER_RADIUS));

	// Direction
	DrawLine3D(component->data.position, endPoint, YELLOW);
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
		RendererModule_GetDefaultMonoFont(),
		text,
		(Vector2) {(float)posX, (float)posY},
		RENDERERMODULE_DEFAULT_FONT_SIZE,
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
	RAYGE_ASSERT(RendererModule_IsInitialised(), "Renderer subsystem must be initialised");

	if ( !entity )
	{
		return;
	}

	if ( g_DebugFlags & RENDERER_DBG_DRAW_LOCATIONS )
	{
		DrawEntityLocation(entity);
	}
}
