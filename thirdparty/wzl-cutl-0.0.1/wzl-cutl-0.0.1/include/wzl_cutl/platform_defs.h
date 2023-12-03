#ifndef WZL_CUTL_PLATFORM_DEFS_H
#define WZL_CUTL_PLATFORM_DEFS_H

#include <stdint.h>

// Constant: WZL_WORD_WIDTH
// Number of bytes required to hold a native register-sized variable.
#if INTPTR_MAX == INT64_MAX
#define WZL_WORD_WIDTH 8
#elif INTPTR_MAX == INT32_MAX
#define WZL_WORD_WIDTH 4
#else
#error Unsupported word width for platform
#endif

// Constant: WZL_IS_64BIT
// Evaluates to true if the system architecture is 64-bit.
#define WZL_IS_64BIT() (WZL_WORD_WIDTH == 8)

// Constant: WZL_IS_32BIT
// Evaluates to true if the system architecture is 32-bit.
#define WZL_IS_32BIT() (WZL_WORD_WIDTH == 4)

#endif  // WZL_CUTL_PLATFORM_DEFS_H
