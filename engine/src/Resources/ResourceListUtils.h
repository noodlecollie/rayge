#pragma once

#include <stdbool.h>
#include "RayGE/ResourceHandle.h"
#include "Resources/ResourceList.h"

typedef bool (*ResourceListUtils_CreationCallback)(const char* relPath, void* itemData, void* userData);

RayGE_ResourceHandle ResourceListUtils_CreateNewItem(
	const char* itemType,
	ResourceList* list,
	const char* relPath,
	ResourceListUtils_CreationCallback callback,
	void* userData
);
