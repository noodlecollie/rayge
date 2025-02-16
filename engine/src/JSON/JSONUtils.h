#pragma once

#include "cJSON.h"
#include "wzl_cutl/attributes.h"

cJSON* JSONUtils_ExpectObjectItem(const char* context, cJSON* parent, const char* key);
cJSON* JSONUtils_ExpectStringItem(const char* context, cJSON* parent, const char* key);

WZL_ATTR_NODISCARD cJSON* JSONUtils_LoadFromFile(const char* relPath);
