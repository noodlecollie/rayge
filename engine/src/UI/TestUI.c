#include <stddef.h>
#include "UI/TestUI.h"
#include "Nuklear/Nuklear.h"
#include "Subsystems/LoggingSubsystem.h"

static void Show(void* userData)
{
	(void)userData;
	LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "TestUI Show()");
}

static void Hide(void* userData)
{
	(void)userData;
	LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "TestUI Hide()");
}

static void Poll(void* userData)
{
	(void)userData;

	struct nk_context* ctx = UISubsystem_GetNuklearContext();

	if ( nk_begin(
			 ctx,
			 "Test Window",
			 nk_rect(50, 50, 800, 600),
			 NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE
		 ) )
	{
		nk_layout_row_dynamic(ctx, 25, 1);

		static int value = 4;
		nk_property_int(ctx, "Test", 0, &value, 10, 1, 1);

		nk_end(ctx);
	}
}

const RayGE_UIMenu Menu_TestUI = {
	NULL,

	Show,
	Hide,
	Poll,
};
