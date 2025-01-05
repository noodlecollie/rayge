#include "cimgui_assert.h"

static cimgui_assert_func g_AssertFunc;

EXTERN_C void cimgui_set_assert_handler(cimgui_assert_func func)
{
	g_AssertFunc = func;
}

EXTERN_C void cimgui_assert(bool expression, const char* description, const char* file, int line, const char* function)
{
	if ( g_AssertFunc )
	{
		g_AssertFunc(expression, description, file, line, function);
	}
}
