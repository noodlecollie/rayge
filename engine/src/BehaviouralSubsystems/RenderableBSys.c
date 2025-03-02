#include "BehaviouralSubsystems/RenderableBSys.h"
#include "Debugging.h"
#include "Headless.h"

#if !RAYGE_HEADLESS()
#include "Non-Headless/EngineSubsystems/RendererSubsystem.h"
#include "Non-Headless/EngineSubsystems/UISubsystem.h"
#include "EngineSubsystems/SceneSubsystem.h"
#endif

#if !RAYGE_HEADLESS()
// TODO: Remove this once rendering is formalised more
static void VisualiseEntities(RayGE_Renderer* renderer)
{
	RayGE_Scene* scene = SceneSubsystem_GetScene();
	RayGE_Entity* firstEnt = Scene_GetActiveEntity(scene, 0);

	if ( firstEnt )
	{
		RayGE_ComponentHeader* cmpHeader = Entity_GetFirstComponentOfType(firstEnt, RAYGE_COMPONENTTYPE_SPATIAL);

		if ( cmpHeader )
		{
			COMPONENTDATA_SPATIAL(cmpHeader)->angles.yaw += 3.0f;
		}
	}

	Renderer_SetDrawingMode3D(renderer, Renderer_GetDefaultCamera3D());
	Renderer_DrawAllActiveEntitiesInScene3D(renderer);
}
#endif

static void Init(void)
{
	// TODO
}

static void ShutDown(void)
{
	// TODO
}

static void Invoke(BSys_Stage stage)
{
	RAYGE_ASSERT(stage == BSYS_STAGE_RENDERING, "Renderable BSys called in state other than BSYS_STAGE_RENDERING!");

	if ( stage != BSYS_STAGE_RENDERING )
	{
		return;
	}

#if !RAYGE_HEADLESS()
	RayGE_Renderer* renderer = RendererSubsystem_GetRenderer();

	// Frame setup
	Renderer_SetBackgroundColour(renderer, BLACK);

	// Begin rendering frame
	Renderer_BeginFrame(renderer);

	// TODO: Remove this once rendering is formalised more
	VisualiseEntities(renderer);

	UISubsystem_Draw();

	// End rendering frame
	Renderer_EndFrame(renderer);
#endif
}

const BSys_Definition RenderableBSys_Definition =
{
	BSYS_STAGE_FLAG(BSYS_STAGE_RENDERING),
	Init,
	ShutDown,
	Invoke,
};
