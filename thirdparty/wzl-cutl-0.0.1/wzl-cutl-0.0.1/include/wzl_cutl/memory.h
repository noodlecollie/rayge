#ifndef WZL_CUTL_MEMORY_H
#define WZL_CUTL_MEMORY_H

#include <stddef.h>
#include "wzl_cutl/libexport.h"

// Struct: wzl_memory_delegates
// Struct holding function pointers which may supply
// overrides for memory allocation functions.
typedef struct wzl_memory_delegates
{
	// Property: malloc_func
	// Pointer to function to be called instead of system malloc().
	void* (*malloc_func)(size_t size);

	// Property: free_func
	// Pointer to function to be called instead of system free().
	void (*free_func)(void* ptr);

	// Pointer: callc_func
	// Pointer to function to be called instead of system calloc().
	void* (*calloc_func)(size_t num, size_t size);

	// Pointer: realloc_func
	// Pointer to function to be called instead of system realloc().
	void* (*realloc_func)(void* ptr, size_t size);
} wzl_memory_delegates;

// Function: wzl_set_memory_delegates
// Sets the static copy of memory delegate functions used internally by the library.
//
// For any delegate in the provided struct, if the function pointer is set
// then this function is called instead of the respective system library function.
// If the function pointer is not set, the system library function is called instead.
//
// Calling this function with a zeroed delegates struct will essentially reset
// all memory function bindings, meaning the system library functions will be
// used instead.
//
// Before the first call to this function, no memory delegates are set
// and the system library functions are used by default.
//
// Parameters:
//
//   delegates - Struct containing delegate function pointers that
//               should be set.
//
// Note:
//
//   This function is not thread-safe.
WZL_CUTL_PUBLIC(void) wzl_set_memory_delegates(wzl_memory_delegates delegates);

#endif  // WZL_CUTL_MEMORY_H
