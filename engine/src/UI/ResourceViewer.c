#include "UI/ResourceViewer.h"
#include "cimgui.h"
#include "wzl_cutl/string.h"
#include "Rendering/TextureResources.h"

static bool g_WindowActive = false;

static void Show(void* userData)
{
	bool* windowActive = (bool*)userData;
	*windowActive = true;
}

static void Hide(void* userData)
{
	bool* windowActive = (bool*)userData;
	*windowActive = false;
}

static bool Poll(void* userData)
{
	bool* windowActive = (bool*)userData;

	if ( *windowActive )
	{
		if ( igBegin("Resource Viewer", windowActive, ImGuiWindowFlags_None) )
		{
			char numTexturesLabel[32];
			wzl_sprintf(numTexturesLabel, sizeof(numTexturesLabel), "Textures: %zu", TextureResources_NumTextures());
			igSeparatorText(numTexturesLabel);

			if ( igBeginTable(
					 "Testing",
					 2,
					 ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter,
					 (ImVec2) {0.0f, 0.0f},
					 0.0f
				 ) )
			{
				igTableSetupColumn("Path", ImGuiTableColumnFlags_None, 0.0f, 0);
				igTableSetupColumn("Dimensions", ImGuiTableColumnFlags_None, 0.0f, 0);
				igTableSetupScrollFreeze(0, 1);
				igTableHeadersRow();

				for ( size_t index = 0; index < 3; ++index )
				{
					igTableNextRow(0, 0.0f);
					igTableNextColumn();

					char text[32];
					wzl_sprintf(text, sizeof(text), "%zu##xx", index);

					igSelectable_Bool(text, false, 0, (ImVec2){0.0f, 0.0f});
					igTableNextColumn();
					igSelectable_Bool(text, false, 0, (ImVec2){0.0f, 0.0f});
				}
			}

			igEndTable();
		}

		igEnd();
	}

	return *windowActive;
}

const RayGE_UIMenu Menu_ResourceViewer = {
	&g_WindowActive,

	NULL,  // Init
	NULL,  // ShutDown
	Show,
	Hide,
	Poll,
};
