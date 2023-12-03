#include "wzl_cutl/libloader.h"
#include "wzl_cutl_/windows/winapi_min.h"
#include <libloaderapi.h>

void* wzl_load_library(const char* filename)
{
	return (void*)LoadLibraryA(filename);
}

bool wzl_unload_library(void* handle)
{
	return FreeLibrary(handle);
}

const char* wzl_get_last_library_error(void)
{
	static char error_buffer[512];

	DWORD error_id = GetLastError();

	if ( error_id == 0 )
	{
		error_buffer[0] = '\0';
		return error_buffer;
	}

	FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_id,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		error_buffer,
		sizeof(error_buffer),
		NULL
	);

	error_buffer[sizeof(error_buffer - 1)] = '\0';
	return error_buffer;
}

void* wzl_get_library_symbol(void* handle, const char* symbol)
{
	return (void*)GetProcAddress(handle, symbol);
}
