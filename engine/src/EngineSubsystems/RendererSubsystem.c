#include <stdbool.h>
#include "EngineSubsystems/RendererSubsystem.h"
#include "MemPool/MemPoolManager.h"
#include "BinaryResources/DMMono_Regular.h"
#include "BinaryResources/OpenSans_Medium.h"
#include "Rendering/TextureResources.h"
#include "Debugging.h"
#include "raylib.h"

typedef struct SubsystemData
{
	Font defaultMonoFont;
	Font defaultUIFont;
	RayGE_Renderer* renderer;
} SubsystemData;

static SubsystemData* g_Data = NULL;

void RendererSubsystem_Init(void)
{
	if ( g_Data )
	{
		return;
	}

	g_Data = MEMPOOL_CALLOC_STRUCT(MEMPOOL_RENDERER, SubsystemData);

	// TODO: Load title from game data
	InitWindow(640, 480, "");
	RAYGE_ENSURE(IsWindowReady(), "Could not create underlying window and OpenGL context");

	// Must be done after window is created, or context will not be able to return the monitor dimensions.
	SetWindowSize(GetMonitorWidth(0) / 2, GetMonitorHeight(0) / 2);

	g_Data->defaultMonoFont = LoadFontFromMemory(
		".ttf",
		TTF_DMMono_Regular_Data,
		TTF_DMMONO_REGULAR_LENGTH,
		RENDERERMODULE_DEFAULT_FONT_SIZE,
		NULL,
		0
	);

	RAYGE_ENSURE(IsFontReady(g_Data->defaultMonoFont), "Could not create default renderer font");

	g_Data->defaultUIFont = LoadFontFromMemory(
		".ttf",
		TTF_OpenSans_Medium_Data,
		TTF_OPENSANS_MEDIUM_LENGTH,
		RENDERERMODULE_DEFAULT_FONT_SIZE,
		NULL,
		0
	);

	RAYGE_ENSURE(IsFontReady(g_Data->defaultUIFont), "Could not create default renderer font");

	g_Data->renderer = Renderer_Create();

	SetExitKey(KEY_NULL);

	// TODO: Make this configurable?
	SetTargetFPS(60);
}

void RendererSubsystem_ShutDown(void)
{
	if ( !g_Data )
	{
		return;
	}

	Renderer_Destroy(g_Data->renderer);
	TextureResources_UnloadAll();
	UnloadFont(g_Data->defaultMonoFont);
	UnloadFont(g_Data->defaultUIFont);
	CloseWindow();

	g_Data = NULL;
}

bool RendererSubsystem_IsInitialised(void)
{
	return g_Data != NULL;
}

bool RendererSubsystem_IsWindowCloseRequested(void)
{
	RAYGE_ASSERT_VALID(g_Data);
	return g_Data && WindowShouldClose();
}

RayGE_Renderer* RendererSubsystem_GetRenderer(void)
{
	RAYGE_ASSERT_VALID(g_Data);

	return g_Data ? g_Data->renderer : NULL;
}

Font RendererSubsystem_GetDefaultMonoFont(void)
{
	RAYGE_ASSERT_VALID(g_Data);

	if ( !g_Data )
	{
		return (Font) {0};
	}

	return g_Data->defaultMonoFont;
}

Font RendererSubsystem_GetDefaultUIFont(void)
{
	RAYGE_ASSERT_VALID(g_Data);

	if ( !g_Data )
	{
		return (Font) {0};
	}

	return g_Data->defaultUIFont;
}
