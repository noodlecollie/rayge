#pragma once

#include <stddef.h>
#include "wzl_cutl/attributes.h"

typedef struct PixelWorld PixelWorld;

WZL_ATTR_NODISCARD PixelWorld* PixelWorld_Create(const char* jsonPath);
void PixelWorld_Destroy(PixelWorld* world);
