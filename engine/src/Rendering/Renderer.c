#include <stdarg.h>
#include "Rendering/Renderer.h"
#include "Modules/RendererModule.h"
#include "Modules/MemPoolModule.h"
#include "Modules/SceneModule.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Debugging.h"

#define DBG_LOCATION_MARKER_RADIUS 4.0f
#define MAX_DEV_TEXT_LENGTH 256

typedef enum RenderCameraType
{
	RENDERCAMERA_NONE = 0,
	RENDERCAMERA_2D,
	RENDERCAMERA_3D
} RenderCameraType;

typedef union RenderCamera
{
	Camera2D cam2D;
	Camera3D cam3D;
} RenderCamera;

struct RayGE_Renderer
{
	uint64_t debugFlags;
	Color backgroundColour;

	RenderCameraType cameraType;
	RenderCamera camera;
	bool inFrame;
};

static Camera3D Default3DCamera(void)
{
	return (Camera3D) {
		{0.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		45.0f,
		CAMERA_PERSPECTIVE,
	};
}

static bool VerifyNotInFrame(RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);
	RAYGE_ASSERT_EXPECT(renderer && !renderer->inFrame, "Cannot perform operation after frame has begun");

	return renderer && !renderer->inFrame;
}

static bool VerifyInFrame(RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);
	RAYGE_ASSERT_EXPECT(renderer && renderer->inFrame, "Cannot perform operation before frame has begun");

	return renderer && renderer->inFrame;
}

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

RayGE_Renderer* Renderer_Create(void)
{
	RayGE_Renderer* renderer = MEMPOOL_CALLOC_STRUCT(MEMPOOL_RENDERER, RayGE_Renderer);

	renderer->debugFlags = 0;
	renderer->backgroundColour = RENDERCOLOUR_NONE;

	renderer->cameraType = RENDERCAMERA_NONE;
	renderer->camera.cam3D = Default3DCamera();

	return renderer;
}

void Renderer_Destroy(RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);

	if ( !renderer )
	{
		return;
	}

	MEMPOOL_FREE(renderer);
}

void Renderer_AddDebugFlags(RayGE_Renderer* renderer, uint64_t flags)
{
	RAYGE_ASSERT_VALID(renderer);

	if ( !renderer )
	{
		return;
	}

	renderer->debugFlags |= flags;
}

void Renderer_RemoveDebugFlags(RayGE_Renderer* renderer, uint64_t flags)
{
	RAYGE_ASSERT_VALID(renderer);

	if ( !renderer )
	{
		return;
	}

	renderer->debugFlags &= ~flags;
}

void Renderer_SetDebugFlags(RayGE_Renderer* renderer, uint64_t flags)
{
	RAYGE_ASSERT_VALID(renderer);

	if ( !renderer )
	{
		return;
	}

	renderer->debugFlags = flags;
}

void Renderer_ClearDebugFlags(RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);

	if ( !renderer )
	{
		return;
	}

	renderer->debugFlags = 0;
}

uint64_t Renderer_GetDebugFlags(const RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);
	return renderer ? renderer->debugFlags : 0;
}

void Renderer_Set2DCamera(RayGE_Renderer* renderer, Camera2D camera)
{
	if ( !VerifyNotInFrame(renderer) )
	{
		return;
	}

	renderer->cameraType = RENDERCAMERA_2D;
	renderer->camera.cam2D = camera;
}

void Renderer_Set3DCamera(RayGE_Renderer* renderer, Camera3D camera)
{
	if ( !VerifyNotInFrame(renderer) )
	{
		return;
	}

	renderer->cameraType = RENDERCAMERA_3D;
	renderer->camera.cam3D = camera;
}

void Renderer_ClearCamera(RayGE_Renderer* renderer)
{
	if ( !VerifyNotInFrame(renderer) )
	{
		return;
	}

	renderer->cameraType = RENDERCAMERA_NONE;
	renderer->camera.cam3D = Default3DCamera();
}

void Renderer_SetBackgroundColour(RayGE_Renderer* renderer, Color colour)
{
	if ( !VerifyNotInFrame(renderer) )
	{
		return;
	}

	renderer->backgroundColour = colour;
}

void Renderer_ClearBackgroundColour(RayGE_Renderer* renderer)
{
	if ( !VerifyNotInFrame(renderer) )
	{
		return;
	}

	renderer->backgroundColour = RENDERCOLOUR_NONE;
}

void Renderer_BeginFrame(RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);
	RAYGE_ASSERT_VALID(!renderer->inFrame);

	if ( !renderer || renderer->inFrame )
	{
		return;
	}

	renderer->inFrame = true;

	if ( renderer->backgroundColour.a > 0 )
	{
		ClearBackground(renderer->backgroundColour);
	}

	switch ( renderer->cameraType )
	{
		case RENDERCAMERA_2D:
		{
			BeginMode2D(renderer->camera.cam2D);
			break;
		}

		case RENDERCAMERA_3D:
		{
			BeginMode3D(renderer->camera.cam3D);
			break;
		}

		default:
		{
			break;
		}
	}
}

void Renderer_EndFrame(RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);
	RAYGE_ASSERT_VALID(renderer->inFrame);

	if ( !renderer || !renderer->inFrame )
	{
		return;
	}

	switch ( renderer->cameraType )
	{
		case RENDERCAMERA_2D:
		{
			EndMode2D();
			break;
		}

		case RENDERCAMERA_3D:
		{
			EndMode3D();
			break;
		}

		default:
		{
			break;
		}
	}

	renderer->inFrame = false;
}

void Renderer_DrawTextDev(RayGE_Renderer* renderer, int posX, int posY, Color color, const char* text)
{
	RAYGE_ASSERT_VALID(renderer);

	if ( !renderer )
	{
		return;
	}

	DrawTextEx(
		RendererModule_GetDefaultMonoFont(),
		text,
		(Vector2) {(float)posX, (float)posY},
		RENDERERMODULE_DEFAULT_FONT_SIZE,
		1.0f,
		color
	);
}

void Renderer_FormatTextDev(RayGE_Renderer* renderer, int posX, int posY, Color color, const char* format, ...)
{
	RAYGE_ASSERT_VALID(renderer);

	if ( !renderer )
	{
		return;
	}

	char buffer[MAX_DEV_TEXT_LENGTH];

	va_list args;
	va_start(args, format);
	wzl_vsprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	Renderer_DrawTextDev(renderer, posX, posY, color, buffer);
}

void Renderer_DrawEntity(RayGE_Renderer* renderer, RayGE_Entity* entity)
{
	if ( !VerifyInFrame(renderer) )
	{
		return;
	}

	RAYGE_ASSERT_VALID(entity);

	if ( !entity )
	{
		return;
	}

	if ( renderer->debugFlags & RENDERER_DBG_DRAW_LOCATIONS )
	{
		DrawEntityLocation(entity);
	}

	// TODO: Proper drawing here
}

void Renderer_DrawAllActiveEntities(RayGE_Renderer* renderer)
{
	if ( !VerifyInFrame(renderer) )
	{
		return;
	}

	RayGE_Scene* scene = SceneModule_GetScene();
	const size_t maxEntities = Scene_GetMaxEntities(scene);

	for ( size_t index = 0; index < maxEntities; ++index )
	{
		RayGE_Entity* entity = Scene_GetActiveEntity(scene, index);

		if ( !entity )
		{
			continue;
		}

		Renderer_DrawEntity(renderer, entity);
	}
}
