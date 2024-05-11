#include "Subsystems/UISubsystem.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Debugging.h"
#include "Nuklear/Nuklear.h"

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

	nk_init(&g_NKContext, &allocator, NULL /*TODO: Font*/);
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
