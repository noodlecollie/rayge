#pragma once

#ifdef RAYMATH_H
#error Do not include raymath.h manually! Include this file instead.
#endif

#include <stdint.h>

#define RAYMATH_STATIC_INLINE
#include "RayGE/External/raymath.h"

typedef struct Vector2i
{
	int32_t x;
	int32_t y;
} Vector2i;
