#pragma once

#ifdef RAYGE_ENABLE_LEAK_CHECK
#ifdef _MSC_VER

#ifndef _DEBUG
#error Leak checking does not function in non-debug builds!
#endif  // _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#error Leak checking is not currently implemented for non-MSVC compilation.

#endif  // _MSC_VER
#endif  // RAYGE_ENABLE_LEAK_CHECK
