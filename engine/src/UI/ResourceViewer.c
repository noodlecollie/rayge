#include "UI/ResourceViewer.h"
#include "cimgui.h"
#include "wzl_cutl/string.h"
#include "Rendering/TextureResources.h"
#include "EngineSubsystems/FilesystemSubsystem.h"

typedef struct Data
{
	bool active;
	Texture2D selectedTexture;
} Data;

static Data g_Data;

static void Show(void* userData)
{
	Data* data = (Data*)userData;
	data->active = true;
}

static void Hide(void* userData)
{
	Data* data = (Data*)userData;
	data->active = false;
	memset(&data->selectedTexture, 0, sizeof(data->selectedTexture));
}

static bool Poll(void* userData)
{
	Data* data = (Data*)userData;

	if ( data->active )
	{
		if ( igBegin("Resource Viewer", &data->active, ImGuiWindowFlags_None) )
		{
			char numTexturesLabel[32];
			wzl_sprintf(numTexturesLabel, sizeof(numTexturesLabel), "Textures: %zu", TextureResources_NumTextures());
			igSeparatorText(numTexturesLabel);

			ImVec2 region = {0.0f, 0.0f};
			igGetContentRegionAvail(&region);

			if ( igBeginTable(
					 "Testing",
					 2,
					 ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter,
					 (ImVec2) {-1.0f * (region.x / 2.0f), 0.0f},
					 0.0f
				 ) )
			{
				igTableSetupColumn("Path", ImGuiTableColumnFlags_None, 0.0f, 0);
				igTableSetupColumn("Dimensions", ImGuiTableColumnFlags_None, 0.0f, 0);
				igTableSetupScrollFreeze(0, 1);
				igTableHeadersRow();

				TextureResources_Iterator iterator = TextureResources_CreateBeginIterator();
				size_t index = 0;

				while ( TextureResourcesIterator_IsValid(iterator) )
				{
					bool clicked = false;
					igTableNextRow(0, 0.0f);
					igTableNextColumn();

					char path[FILESYSTEM_MAX_REL_PATH + 16];
					wzl_sprintf(path, sizeof(path), "%s##%zu", TextureResourcesIterator_GetPath(iterator), index);

					if ( igSelectable_Bool(path, false, 0, (ImVec2) {0.0f, 0.0f}) )
					{
						clicked = true;
					}

					igTableNextColumn();

					char dimensions[64];
					Texture2D texture = TextureResourcesIterator_GetTexture(iterator);

					wzl_sprintf(dimensions, sizeof(dimensions), "%dx%d##%zu", texture.width, texture.height, index);

					if ( igSelectable_Bool(dimensions, false, 0, (ImVec2) {0.0f, 0.0f}) )
					{
						clicked = true;
					}

					if ( clicked )
					{
						data->selectedTexture = texture;
					}

					iterator = TextureResources_IncrementIterator(iterator);
					++index;
				}
			}

			igEndTable();
			igSameLine(0.0f, -1.0f);

			if ( data->selectedTexture.id > 0 )
			{
				igImage(
					data->selectedTexture.id,
					(ImVec2) {(float)data->selectedTexture.width, (float)data->selectedTexture.height},
					(ImVec2) {0.0f, 0.0f},
					(ImVec2) {1.0f, 1.0f},
					(ImVec4) {1.0f, 1.0f, 1.0f, 1.0f},
					(ImVec4) {0.0f, 0.0f, 0.0f, 0.0f}
				);
			}
		}

		igEnd();
	}

	return data->active;
}

const RayGE_UIMenu Menu_ResourceViewer = {
	&g_Data,

	NULL,  // Init
	NULL,  // ShutDown
	Show,
	Hide,
	Poll,
};
