#pragma once

#include <stdint.h>

void* Platform_LoadLibrary(const char* filename, uint32_t flags);
int32_t Platform_CloseLibrary(void* handle);
const char* Platform_GetLastLibraryError(void);
void* Platform_LookUpLibrarySymbol(void* handle, const char* symbol);
