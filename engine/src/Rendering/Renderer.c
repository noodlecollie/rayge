#include <stdarg.h>
#include <limits.h>
#include "Rendering/Renderer.h"
#include "RayGE/External/raymath.h"
#include "Rendering/RenderablePrimitives.h"
#include "EngineSubsystems/RendererSubsystem.h"
#include "MemPool/MemPoolManager.h"
#include "EngineSubsystems/SceneSubsystem.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "ResourceManagement/ResourceHandleUtils.h"
#include "Conversions.h"
#include "Debugging.h"
#include "raylib.h"
#include "rlgl.h"
#include "cimgui.h"

#define DBG_LOCATION_MARKER_RADIUS 4.0f
#define MAX_DEV_TEXT_LENGTH 256

typedef enum DrawMode
{
	DRAWMODE_DIRECT = 0,
	DRAWMODE_2D,
	DRAWMODE_3D
} DrawMode;

struct RayGE_Renderer
{
	uint64_t debugFlags;
	Color backgroundColour;

	bool inFrame;
	DrawMode drawMode;
	Camera2D cam2D;
	Camera3D cam3D;

	// Only used if the "override camera" debug flag is set
	Camera2D debugCam2D;
	Camera3D debugCam3D;
};

static Camera2D Default2DCamera(void)
{
	return (Camera2D) {
		.offset = {0.0f, 0.0f},
		.target = {0.0f, 0.0f},
		.rotation = 0.0f,
		.zoom = 1.0f,
	};
}

static Camera3D Default3DCamera(void)
{
	return (Camera3D) {
		.position = {0.0f, 0.0f, 0.0f},
		.target = {1.0f, 0.0f, 0.0f},
		.up = {0.0f, 0.0f, 1.0f},
		.fovy = 45.0f,
		.projection = CAMERA_PERSPECTIVE,
	};
}

static bool VerifyNotInFrame(const RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);
	RAYGE_ASSERT_EXPECT(renderer && !renderer->inFrame, "Cannot perform operation after frame has begun");

	return renderer && !renderer->inFrame;
}

static bool VerifyInFrame(const RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);
	RAYGE_ASSERT_EXPECT(renderer && renderer->inFrame, "Cannot perform operation before frame has begun");

	return renderer && renderer->inFrame;
}

static bool VerifyInDrawMode(const RayGE_Renderer* renderer, DrawMode mode)
{
	if ( !VerifyInFrame(renderer) )
	{
		return false;
	}

	RAYGE_ASSERT_VALID(renderer->drawMode == mode);
	return renderer->drawMode == mode;
}

static Camera2D GetCamera2D(const RayGE_Renderer* renderer)
{
	return (renderer->debugFlags & RENDERER_DBG_OVERRIDE_CAMERA) ? renderer->debugCam2D : renderer->cam2D;
}

static Camera3D GetCamera3D(const RayGE_Renderer* renderer)
{
	return (renderer->debugFlags & RENDERER_DBG_OVERRIDE_CAMERA) ? renderer->debugCam3D : renderer->cam3D;
}

static void TransitionToDrawMode(RayGE_Renderer* renderer, DrawMode mode)
{
	if ( renderer->drawMode == mode )
	{
		return;
	}

	if ( renderer->inFrame )
	{
		switch ( renderer->drawMode )
		{
			case DRAWMODE_2D:
			{
				EndMode2D();
				break;
			}

			case DRAWMODE_3D:
			{
				EndMode3D();
				break;
			}

			default:
			{
				break;
			}
		}
	}

	renderer->drawMode = mode;

	if ( renderer->inFrame )
	{
		switch ( renderer->drawMode )
		{
			case DRAWMODE_2D:
			{
				BeginMode2D(GetCamera2D(renderer));
				break;
			}

			case DRAWMODE_3D:
			{
				BeginMode3D(GetCamera3D(renderer));
				break;
			}

			default:
			{
				break;
			}
		}
	}
}

static void DrawRawVertex2D(const Renderer_RawVertex2D* vertex)
{
	rlColor4ub(vertex->col.r, vertex->col.g, vertex->col.b, vertex->col.a);
	rlTexCoord2f(vertex->uv.x, vertex->uv.y);
	rlVertex2f(vertex->pos.x, vertex->pos.y);
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

static void DrawRenderablePrimitive(
	RayGE_RenderablePrimitive primitive,
	const RayGE_Component_Renderable* renderable,
	Vector3 position
)
{
	switch ( primitive )
	{
		case RAYGE_RENDERABLE_PRIM_SPHERE:
		{
			DrawSphere(position, renderable->scale, PublicToRaylibColor(renderable->color));
			break;
		}

		case RAYGE_RENDERABLE_PRIM_AACUBE:
		{
			DrawCube(
				position,
				renderable->scale,
				renderable->scale,
				renderable->scale,
				PublicToRaylibColor(renderable->color)
			);
			break;
		}

		default:
		{
			RAYGE_ASSERT_UNREACHABLE("Unknown renderable primitive type %d", primitive);
			break;
		}
	}
}

static void DrawRenderable(const RayGE_Component_Renderable* renderable, Vector3 position)
{
	if ( !renderable )
	{
		return;
	}

	switch ( Resource_GetInternalDomain(renderable->handle) )
	{
		case RESOURCE_DOMAIN_RENDERABLE_PRIMITIVE:
		{
			DrawRenderablePrimitive(
				RenderablePrimitive_GetPrimitiveFromHandle(renderable->handle),
				renderable,
				position
			);
			break;
		}

		default:
		{
			break;
		}
	}
}

RayGE_Renderer* Renderer_Create(void)
{
	RayGE_Renderer* renderer = MEMPOOL_CALLOC_STRUCT(MEMPOOL_RENDERER, RayGE_Renderer);

	renderer->debugFlags = 0;
	renderer->backgroundColour = RENDERCOLOUR_NONE;
	renderer->drawMode = DRAWMODE_DIRECT;
	renderer->cam2D = Default2DCamera();
	renderer->debugCam2D = Default2DCamera();
	renderer->cam3D = Default3DCamera();
	renderer->debugCam3D = Default3DCamera();

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

Camera2D Renderer_GetDefaultCamera2D(void)
{
	return Default2DCamera();
}

Camera3D Renderer_GetDefaultCamera3D(void)
{
	return Default3DCamera();
}

Camera2D Renderer_GetDebugCamera2D(const RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);
	return renderer ? renderer->debugCam2D : Default2DCamera();
}

void Renderer_SetDebugCamera2D(RayGE_Renderer* renderer, Camera2D camera)
{
	if ( !VerifyNotInFrame(renderer) )
	{
		return;
	}

	renderer->debugCam2D = camera;
}

void Renderer_ResetDebugCamera2D(RayGE_Renderer* renderer)
{
	Renderer_SetDebugCamera2D(renderer, Default2DCamera());
}

Camera3D Renderer_GetDebugCamera3D(const RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);
	return renderer ? renderer->debugCam3D : Default3DCamera();
}

void Renderer_SetDebugCamera3D(RayGE_Renderer* renderer, Camera3D camera)
{
	if ( !VerifyNotInFrame(renderer) )
	{
		return;
	}

	renderer->debugCam3D = camera;
}

void Renderer_ResetDebugCamera3D(RayGE_Renderer* renderer)
{
	Renderer_SetDebugCamera3D(renderer, Default3DCamera());
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
	renderer->drawMode = DRAWMODE_DIRECT;

	BeginDrawing();

	if ( renderer->backgroundColour.a > 0 )
	{
		ClearBackground(renderer->backgroundColour);
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

	TransitionToDrawMode(renderer, DRAWMODE_DIRECT);
	EndDrawing();
	renderer->inFrame = false;
}

bool Renderer_IsInFrame(const RayGE_Renderer* renderer)
{
	RAYGE_ASSERT_VALID(renderer);
	return renderer && renderer->inFrame;
}

void Renderer_SetDrawingMode2D(RayGE_Renderer* renderer, Camera2D camera)
{
	if ( !VerifyInFrame(renderer) )
	{
		return;
	}

	renderer->cam2D = camera;
	TransitionToDrawMode(renderer, DRAWMODE_2D);
}

void Renderer_SetDrawingMode3D(RayGE_Renderer* renderer, Camera3D camera)
{
	if ( !VerifyInFrame(renderer) )
	{
		return;
	}

	renderer->cam3D = camera;
	TransitionToDrawMode(renderer, DRAWMODE_3D);
}

void Renderer_SetDrawingModeDirect(RayGE_Renderer* renderer)
{
	if ( !VerifyInFrame(renderer) )
	{
		return;
	}

	TransitionToDrawMode(renderer, DRAWMODE_DIRECT);
}

void Renderer_DrawEntity3D(RayGE_Renderer* renderer, RayGE_Entity* entity)
{
	if ( !VerifyInDrawMode(renderer, DRAWMODE_3D) )
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

	RayGE_ComponentHeader* spatial = Entity_GetFirstComponentOfType(entity, RAYGE_COMPONENTTYPE_SPATIAL);
	RayGE_ComponentHeader* renderable = Entity_GetFirstComponentOfType(entity, RAYGE_COMPONENTTYPE_RENDERABLE);

	Vector3 position = {0.0f, 0.0f, 0.0f};

	if ( spatial )
	{
		position = COMPONENTDATA_SPATIAL(spatial)->position;
	}

	if ( renderable )
	{
		DrawRenderable(COMPONENTDATA_RENDERABLE(renderable), position);
	}
}

void Renderer_DrawAllActiveEntitiesInScene3D(RayGE_Renderer* renderer)
{
	if ( !VerifyInDrawMode(renderer, DRAWMODE_3D) )
	{
		return;
	}

	RayGE_Scene* scene = SceneSubsystem_GetScene();
	const uint32_t maxEntities = Scene_GetMaxEntities(scene);

	for ( uint32_t index = 0; index < maxEntities; ++index )
	{
		RayGE_Entity* entity = Scene_GetActiveEntity(scene, index);

		if ( !entity )
		{
			continue;
		}

		Renderer_DrawEntity3D(renderer, entity);
	}
}

void Renderer_DirectDrawTextDev(RayGE_Renderer* renderer, int posX, int posY, Color color, const char* text)
{
	if ( !VerifyInDrawMode(renderer, DRAWMODE_DIRECT) )
	{
		return;
	}

	DrawTextEx(
		RendererSubsystem_GetDefaultMonoFont(),
		text,
		(Vector2) {(float)posX, (float)posY},
		RENDERERMODULE_DEFAULT_FONT_SIZE,
		1.0f,
		color
	);
}

void Renderer_DirectFormatTextDev(RayGE_Renderer* renderer, int posX, int posY, Color color, const char* format, ...)
{
	if ( !VerifyInDrawMode(renderer, DRAWMODE_DIRECT) )
	{
		return;
	}

	char buffer[MAX_DEV_TEXT_LENGTH];

	va_list args;
	va_start(args, format);
	wzl_vsprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	Renderer_DirectDrawTextDev(renderer, posX, posY, color, buffer);
}

void Renderer_DirectDrawImGui(RayGE_Renderer* renderer)
{
	if ( !VerifyInDrawMode(renderer, DRAWMODE_DIRECT) )
	{
		return;
	}

	ImGuiIO* io = igGetIO();
	ImDrawData* drawData = igGetDrawData();

	rlDrawRenderBatchActive();
	rlDisableBackfaceCulling();

	for ( int cmdListIndex = 0; cmdListIndex < drawData->CmdLists.Size; ++cmdListIndex )
	{
		const ImDrawList* commandList = drawData->CmdLists.Data[cmdListIndex];

		if ( commandList->VtxBuffer.Size < 1 || commandList->IdxBuffer.Size < 1 )
		{
			continue;
		}

		Renderer_RawBatch2D batch;
		memset(&batch, 0, sizeof(batch));

		batch.indices = commandList->IdxBuffer.Data;
		batch.indexCount = (size_t)commandList->IdxBuffer.Size;
		batch.vertexCount = (size_t)commandList->VtxBuffer.Size;

		Renderer_RawVertex2D* vertices = (Renderer_RawVertex2D*)MEMPOOL_MALLOC(MEMPOOL_RENDERER, batch.vertexCount);

		for ( size_t vIndex = 0; vIndex < batch.vertexCount; ++vIndex )
		{
			const ImDrawVert* vSrc = commandList->VtxBuffer.Data;
			Renderer_RawVertex2D* vDest = &vertices[vIndex];

			vDest->col.r = vSrc->col & 0xFF000000;
			vDest->col.g = vSrc->col & 0x00FF0000;
			vDest->col.b = vSrc->col & 0x0000FF00;
			vDest->col.a = vSrc->col & 0x000000FF;

			vDest->pos = (Vector2) {vSrc->pos.x, vSrc->pos.y};
			vDest->uv = (Vector2) {vSrc->uv.x, vSrc->uv.y};
		}

		batch.vertices = vertices;

		for ( int cmdBufferIndex = 0; cmdBufferIndex < commandList->CmdBuffer.Size; ++cmdBufferIndex )
		{
			const ImDrawCmd* cmd = &commandList->CmdBuffer.Data[cmdBufferIndex];

			const Rectangle clipRect = {
				cmd->ClipRect.x - drawData->DisplayPos.x,
				cmd->ClipRect.y - drawData->DisplayPos.y,
				cmd->ClipRect.z - (cmd->ClipRect.x - drawData->DisplayPos.x),
				cmd->ClipRect.w - (cmd->ClipRect.y - drawData->DisplayPos.y)
			};

			rlEnableScissorTest();

			Vector2 scale = {io->DisplayFramebufferScale.x, io->DisplayFramebufferScale.y};

#if !defined(__APPLE__)
			if ( !IsWindowState(FLAG_WINDOW_HIGHDPI) )
			{
				scale.x = 1;
				scale.y = 1;
			}
#endif

			rlScissor(
				(int)(clipRect.x * scale.x),
				(int)((io->DisplaySize.y - (int)(clipRect.y + clipRect.height)) * scale.y),
				(int)(clipRect.width * scale.x),
				(int)(clipRect.height * scale.y)
			);

			if ( cmd->UserCallback )
			{
				cmd->UserCallback(commandList, cmd);
				continue;
			}

			batch.textureID = cmd->TextureId;
			Renderer_DirectDrawTriangles2D(renderer, &batch, cmd->IdxOffset, cmd->ElemCount);

			rlDrawRenderBatchActive();
		}

		MEMPOOL_FREE(vertices);
	}

	rlSetTexture(0);
	rlDisableScissorTest();
	rlEnableBackfaceCulling();
}

void Renderer_DirectDrawTriangles2D(
	RayGE_Renderer* renderer,
	const Renderer_RawBatch2D* batch,
	size_t indexOffset,
	size_t indexCount
)
{
	if ( !VerifyInDrawMode(renderer, DRAWMODE_DIRECT) )
	{
		return;
	}

	RAYGE_ASSERT_VALID(batch);
	RAYGE_ASSERT(batch->vertices && batch->vertexCount > 0, "No vertices were provided");
	RAYGE_ASSERT(batch->indices && batch->indexCount > 0, "No indices were provided");

	if ( !batch || !batch->vertices || batch->vertexCount < 1 || !batch->indices || batch->indexCount < 1 )
	{
		return;
	}

	if ( indexOffset >= batch->indexCount )
	{
		// Nothing to draw.
		return;
	}

	if ( indexCount < 1 || indexCount > batch->indexCount )
	{
		indexCount = batch->indexCount;
	}

	const size_t triangleCount = indexCount / 3;

	if ( triangleCount < 1 )
	{
		// Nothing to draw.
		return;
	}

	rlBegin(RL_TRIANGLES);
	rlSetTexture(batch->textureID);

	for ( size_t triIndex = 0; triIndex < triangleCount; ++triIndex )
	{
		Renderer_Index indexA = batch->indices[(3 * triIndex) + indexOffset];
		Renderer_Index indexB = batch->indices[(3 * triIndex) + indexOffset + 1];
		Renderer_Index indexC = batch->indices[(3 * triIndex) + indexOffset + 2];

		if ( indexA >= batch->vertexCount || indexB >= batch->vertexCount || indexC >= batch->vertexCount )
		{
			RAYGE_ASSERT(false, "One or more indices were out of range of the vertex buffer");
			continue;
		}

		DrawRawVertex2D(&batch->vertices[indexA]);
		DrawRawVertex2D(&batch->vertices[indexB]);
		DrawRawVertex2D(&batch->vertices[indexC]);
	}

	rlEnd();
}
