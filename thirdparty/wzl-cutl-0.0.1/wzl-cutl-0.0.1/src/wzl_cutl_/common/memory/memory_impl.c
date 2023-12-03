#include "wzl_cutl/memory.h"

wzl_memory_delegates g_memory_delegates;

WZL_CUTL_PUBLIC(void) wzl_set_memory_delegates(wzl_memory_delegates delegates)
{
	g_memory_delegates = delegates;
}
