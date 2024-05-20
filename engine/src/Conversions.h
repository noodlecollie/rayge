#pragma once

#include "RayGE/Color.h"
#include "raylib.h"

static inline Color PublicToRaylibColor(RayGE_Color col)
{
	return (Color) {col.r, col.g, col.b, col.a};
}

static inline RayGE_Color RaylibToPublocColor(Color col)
{
	return (RayGE_Color) {col.r, col.g, col.b, col.a};
}
