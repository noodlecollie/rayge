#include "Platform/Library.h"
#include <dlfcn.h>

void* Platform_LoadLibrary(const char* filename)
{
	return dlopen(filename, RTLD_NOW);
}

int32_t Platform_UnloadLibrary(void* handle)
{
	return dlclose(handle);
}

const char* Platform_GetLastLibraryError(void)
{
	return dlerror();
}

void* Platform_LookUpLibrarySymbol(void* handle, const char* symbol)
{
	return dlsym(handle, symbol);
}
