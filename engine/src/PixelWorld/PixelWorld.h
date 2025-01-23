#pragma once

typedef struct PixelWorld PixelWorld;

PixelWorld* PixelWorld_Create(const char* jsonPath);
void PixelWorld_Destroy(PixelWorld* world);
