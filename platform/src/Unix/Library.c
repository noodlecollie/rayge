#include "Platform/Library.h"
#include <dlfcn.h>

void* Platform_LoadLibrary(const char* filename, uint32_t flags)
{
	return dlopen(filename, (int)flags);
}

int32_t Platform_CloseLibrary(void* handle)
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
