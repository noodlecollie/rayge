#pragma once

#ifdef RAYMATH_H
#error Do not include raymath.h manually! Include this file instead.
#endif

#include <stdint.h>

#define RAYMATH_STATIC_INLINE
#include "RayGE/External/raymath.h"

// Define these so that the raylib header does not
#define RL_VECTOR2_TYPE
#define RL_VECTOR3_TYPE
#define RL_VECTOR4_TYPE
#define RL_QUATERNION_TYPE
#define RL_MATRIX_TYPE

typedef struct Vector2i
{
	int32_t x;
	int32_t y;
} Vector2i;
