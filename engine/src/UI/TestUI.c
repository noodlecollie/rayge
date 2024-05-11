#include "UI/TestUI.h"
#include "Nuklear/Nuklear.h"
#include "Subsystems/LoggingSubsystem.h"

static void Show(void)
{
	LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "TestUI Show()");
}

static void Hide(void)
{
	LoggingSubsystem_PrintLine(RAYGE_LOG_INFO, "TestUI Hide()");
}

static void Poll(void)
{
	struct nk_context* ctx = UISubsystem_GetNuklearContext();

	if ( nk_begin(
			 ctx,
			 "Test Window",
			 nk_rect(50, 50, 800, 600),
			 NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_MOVABLE
		 ) )
	{
		nk_button_label(ctx, "Button");

		nk_end(ctx);
	}
}

extern const RayGE_UIMenu Menu_TestUI = {
	Show,
	Hide,
	Poll,
};
