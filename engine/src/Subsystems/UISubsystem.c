#include "Subsystems/UISubsystem.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Subsystems/RendererSubsystem.h"
#include "Debugging.h"
#include "raylib.h"

static struct nk_context g_NKContext;
static bool g_Initialised = false;

static void* LocalAllocate(nk_handle handle, void* ptr, nk_size size)
{
	(void)handle;
	(void)ptr;

	return MEMPOOL_MALLOC(MEMPOOL_UI, size);
}

static void LocalFree(nk_handle handle, void* ptr)
{
	(void)handle;

	MEMPOOL_FREE(ptr);
}

static float ComputeTextWidthForDefaultFont(nk_handle handle, float height, const char* text, int textLength)
{
	(void)handle;
	(void)textLength;

	return MeasureTextEx(RendererSubsystem_GetDefaultFont(), text, height, 0.0f).x;
}

void UISubsystem_Init(void)
{
	if ( g_Initialised )
	{
		return;
	}

	struct nk_allocator allocator = {
		0,
		&LocalAllocate,
		&LocalFree,
	};

	const struct nk_user_font font = {
		0,
		(float)RENDERSUBSYSTEM_DEFAULT_FONT_SIZE,
		&ComputeTextWidthForDefaultFont,
	};

	nk_init(&g_NKContext, &allocator, &font);
	g_Initialised = true;
}

void UISubsystem_ShutDown(void)
{
	if ( !g_Initialised )
	{
		return;
	}

	nk_free(&g_NKContext);
	g_Initialised = false;
}
