#pragma once

#include <stdbool.h>

bool GameData_Load(const char* filePath);
bool GameData_IsLoaded(void);

const char* GameData_GetGameLibraryPath(void);
