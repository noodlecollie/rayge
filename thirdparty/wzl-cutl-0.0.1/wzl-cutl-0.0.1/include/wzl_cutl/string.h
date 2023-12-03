#ifndef WZL_CUTL_STRING_H
#define WZL_CUTL_STRING_H

#include "wzl_cutl/libexport.h"

// Function: wzl_duplicate_string
// Given an input string, makes a dynamic copy of this string
// and returns it. Memory is allocated according to the
// currently set memory delegates.
//
// The caller takes ownership of the allocated string.
// The string must be freed after use to avoid creating
// a memory leak.
//
// If the input string is NULL, no memory allocation
// takes place and NULL is returned.
//
// Parameters:
//
//   str - Input string to create a duplicate of.
//
// Returns:
//
//   Pointer to allocated memory for the duplicated
//   string, or NULL if the input string was NULL.
WZL_CUTL_PUBLIC(char*) wzl_duplicate_string(char* str);

#endif  // WZL_CUTL_STRING_H
