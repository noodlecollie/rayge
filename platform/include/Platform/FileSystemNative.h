#pragma once

#include <stdbool.h>

bool Platform_SetExecutableFromArgV0(const char* nativePath);

// Assumes Platform_SetExecutableFromArgV0() has been called successfully.
// Caller takes ownership of the native path. It must be freed later.
// If the path is null or empty, the executable path is returned.
char* Platform_NativeAbsolutePathFromExecutableDirectory(const char* relativePath);

void Platform_PathSeparatorsToNative(char* path);
