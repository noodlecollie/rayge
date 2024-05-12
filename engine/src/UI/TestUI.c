#include <stddef.h>
#include "UI/TestUI.h"
#include "Nuklear/Nuklear.h"
#include "Subsystems/LoggingSubsystem.h"

nk_bool EditFilter(const struct nk_text_edit* edit, nk_rune unicode)
{
	(void)edit;
	(void)unicode;
	return true;
}

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

		static float floatValue = 0.5f;
		nk_slider_float(ctx, 0, &floatValue, 1.0f, 0.1f);

		nk_text(ctx, "This is some text", 17, NK_TEXT_ALIGN_CENTERED);
		nk_label(ctx, "This is a label", NK_TEXT_ALIGN_CENTERED);

		nk_button_label(ctx, "Button");
		nk_button_symbol_label(ctx, NK_SYMBOL_TRIANGLE_UP, "Button With Triangle", NK_TEXT_ALIGN_LEFT);
		nk_check_label(ctx, "Tick Box", true);

		static nk_bool radioActive = true;
		nk_radio_label(ctx, "Radiobutton", &radioActive);
		nk_option_label(ctx, "Option", true);

		static nk_bool selectableValue = false;
		nk_selectable_label(ctx, "Selectable", NK_TEXT_ALIGN_RIGHT, &selectableValue);
		nk_select_label(ctx, "Select", NK_TEXT_ALIGN_RIGHT, true);
		nk_prog(ctx, 20, 100, true);

		static char editBuffer[32] = "Text box";
		nk_edit_string_zero_terminated(ctx, NK_EDIT_DEFAULT, editBuffer, sizeof(editBuffer), EditFilter);

		const float values[8] = { 0.0f, 1.0f, 2.0f, 1.0f, 0.0f, 1.0f, 2.0f, 1.0f };
		nk_plot(ctx, NK_CHART_LINES, values, 8, 0);

		const char* comboValues[3] = { "First", "Second", "Third" };
		nk_combo(ctx, comboValues, 3, 0, 20, (struct nk_vec2){100, 100});

		nk_end(ctx);
	}
}

const RayGE_UIMenu Menu_TestUI = {
	NULL,

	Show,
	Hide,
	Poll,
};
