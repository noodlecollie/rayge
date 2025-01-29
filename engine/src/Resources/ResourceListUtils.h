#pragma once

#include <stdbool.h>
#include "RayGE/ResourceHandle.h"
#include "Resources/ResourceList.h"
#include "wzl_cutl/attributes.h"

typedef bool (*ResourceListUtils_CreationCallback)(const char* relPath, void* itemData, void* userData);

WZL_ATTR_NODISCARD RayGE_ResourceHandle ResourceListUtils_CreateNewItem(
	const char* itemType,
	ResourceList* list,
	const char* relPath,
	ResourceListUtils_CreationCallback callback,
	void* userData
);
