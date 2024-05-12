#pragma once

#include "cJSON.h"

cJSON* JSONUtils_ExpectObjectItem(const char* context, cJSON* parent, const char* key);
cJSON* JSONUtils_ExpectStringItem(const char* context, cJSON* parent, const char* key);
