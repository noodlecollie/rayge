#pragma once

#ifdef RAYGE_HEADLESS_FLAG
#define RAYGE_HEADLESS() 1
#else
#define RAYGE_HEADLESS() 0
#endif
