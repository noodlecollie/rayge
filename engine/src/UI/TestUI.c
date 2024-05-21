#include <stddef.h>
#include "UI/TestUI.h"
#include "Nuklear/Nuklear.h"
#include "Logging/Logging.h"

nk_bool EditFilter(const struct nk_text_edit* edit, nk_rune unicode)
{
	(void)edit;
	(void)unicode;
	return true;
}

static void Show(struct nk_context* context, void* userData)
{
	(void)context;
	(void)userData;
	Logging_PrintLine(RAYGE_LOG_INFO, "TestUI Show()");
}

static void Hide(struct nk_context* context, void* userData)
{
	(void)context;
	(void)userData;
	Logging_PrintLine(RAYGE_LOG_INFO, "TestUI Hide()");
}

static bool Poll(struct nk_context* context, void* userData)
{
	(void)userData;

	if ( nk_begin(
			 context,
			 "Test Window",
			 nk_rect(50, 50, 800, 600),
			 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE
		 ) )
	{
		nk_layout_row_dynamic(context, 25, 1);

		static int value = 4;
		nk_property_int(context, "Test", 0, &value, 10, 1, 1);

		static float floatValue = 0.5f;
		nk_slider_float(context, 0, &floatValue, 1.0f, 0.1f);

		nk_text(context, "This is some text", 17, NK_TEXT_ALIGN_CENTERED);
		nk_label(context, "This is a label", NK_TEXT_ALIGN_CENTERED);

		nk_button_label(context, "Button");
		nk_button_symbol_label(context, NK_SYMBOL_TRIANGLE_UP, "Button With Triangle", NK_TEXT_ALIGN_LEFT);
		nk_check_label(context, "Tick Box", true);

		static nk_bool radioActive = true;
		nk_radio_label(context, "Radiobutton", &radioActive);
		nk_option_label(context, "Option", true);

		static nk_bool selectableValue = false;
		nk_selectable_label(context, "Selectable", NK_TEXT_ALIGN_RIGHT, &selectableValue);
		nk_select_label(context, "Select", NK_TEXT_ALIGN_RIGHT, true);
		nk_prog(context, 20, 100, true);

		static char editBuffer[32] = "Text box";
		nk_edit_string_zero_terminated(context, NK_EDIT_DEFAULT, editBuffer, sizeof(editBuffer), EditFilter);

		const float values[8] = {0.0f, 1.0f, 2.0f, 1.0f, 0.0f, 1.0f, 2.0f, 1.0f};
		nk_plot(context, NK_CHART_LINES, values, 8, 0);

		const char* comboValues[3] = {"First", "Second", "Third"};
		nk_combo(context, comboValues, 3, 0, 20, (struct nk_vec2) {100, 100});

		nk_color_picker(context, (struct nk_colorf) {1.0f, 0.0f, 0.0f, 1.0f}, NK_RGB);
	}

	nk_end(context);
	return true;
}

const RayGE_UIMenu Menu_TestUI = {
	NULL,

	Show,
	Hide,
	Poll,
};
