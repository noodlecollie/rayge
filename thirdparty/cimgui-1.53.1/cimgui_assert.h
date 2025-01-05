#pragma once

#include <stdbool.h>

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

typedef void (*cimgui_assert_func)(bool /*expression*/, const char* /*description*/, const char* /*file*/, int /*line*/, const char* /*function*/);

EXTERN_C void cimgui_set_assert_handler(cimgui_assert_func func);
