#include "wzl_cutl/libloader.h"
#include <dlfcn.h>

void* wzl_load_library(const char* filename)
{
	return dlopen(filename, RTLD_NOW);
}

bool wzl_unload_library(void* handle)
{
	return dlclose(handle) == 0;
}

const char* wzl_get_last_library_error(void)
{
	return dlerror();
}

void* wzl_get_library_symbol(void* handle, const char* symbol)
{
	return dlsym(handle, symbol);
}
