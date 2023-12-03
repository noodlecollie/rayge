#ifndef WZL_CUTL_LIBLOADER_H
#define WZL_CUTL_LIBLOADER_H

#include <stdbool.h>
#include "wzl_cutl/libexport.h"

// Typedef: wzl_opaque_funcptr
// Alias for a generic function pointer.
//
// Attempting to cast void* to a function pointer
// when compiling with -pedantic on GCC causes a warning.
// <wzl_opaque_funcptr> is intended to represent a general
// function pointer which can be cast to different function
// pointers without generating warnings.
typedef void (*wzl_opaque_funcptr)(void);

// Function: wzl_load_library
// Attempts to load a library from the system.
//
// If the library cannot be loaded, NULL is returned.
// <wzl_get_last_library_error> can be used to fetch
// a string describing the error that occurred.
//
// Parameters:
//
//   filename - Name of library to load.
//
// Returns:
//
//   Handle to library, or NULL if the library could
//   not be loaded.
void* wzl_load_library(const char* filename);

// Function: wzl_unload_library
// Unloads a library that has previously been loaded.
//
// If the passed handle was not returned from a previous
// call to <wzl_load_library>, behaviour is undefined.
//
// Parameters:
//
//   handle - Handle to a previously loaded library.
bool wzl_unload_library(void* handle);

// Function: wzl_get_last_library_error
// Returns a string representing the last error that
// occurred when calling a library-related function.
//
// Returns:
//
//   Human-readable string describing the last error.
const char* wzl_get_last_library_error(void);

// Function: wzl_get_library_symbol
// Returns a pointer to the symbol with the specified
// name within the specified library.
//
// If the function is not able to be looked up, NULL
// is returned. <wzl_get_last_library_error> can be used
// to fetch a string describing the error that occurred.
//
// Note:
//
//   If compiling with -pedantic, the compiler may warn about
//   casting the return value of this function to a function
//   pointer. There is no canonical way of avoiding this,
//   other than suppressing the -pedantic diagnostic for the
//   cast. <wzl_get_library_function> is a convenience
//   function that will do this for you.
//
// Parameters:
//
//   handle - Handle to the library which holds the symbol.
//   name   - Name of the symbol to look up.
//
// Returns:
//
//   Pointer to the requested symbol, or NULL if it could not
//   be obtained.
void* wzl_get_library_symbol(void* handle, const char* symbol);

// Function: wzl_get_library_function
// Returns a pointer to the function with the specified
// name within the specified library.
//
// If compiling with -pedantic, the compiler may warn about
// casting a void* pointer to a function pointer. There is
// no canonical way of avoiding this, other than suppressing
// the -pedantic diagnostic for the cast. This function wraps
// the diagnostic suppression, for convenience; in all other
// respects, it behaves identically to <wzl_get_library_symbol>.
//
// Note:
//
//   The return value of this function is simply a function
//   pointer that returns void and takes no arguments.
//   It is expected that the caller casts this function
//   pointer to the intended signature.
//
// Parameters:
//
//   handle - Handle to the library which holds the function.
//   name   - Name of the function to look up.
//
// Returns:
//
//   Pointer to the requested function, or NULL if it could not
//   be obtained.
static inline wzl_opaque_funcptr wzl_get_library_function(void* handle, const char* name)
{
#ifndef _MSC_VER
// No way to cast between void* and a function pointer without suppressing -pedantic...
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif // _MSC_VER

	return (wzl_opaque_funcptr)wzl_get_library_symbol(handle, name);

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif // _MSC_VER
}

#endif  // WZL_CUTL_LIBLOADER_H
