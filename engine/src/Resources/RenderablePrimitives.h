#pragma once

#include "RayGE/ResourceHandle.h"
#include "RayGE/APIs/Resources.h"

RayGE_ResourceHandle RenderablePrimitive_GetHandle(RayGE_RenderablePrimitive primitive);
RayGE_RenderablePrimitive RenderablePrimitive_GetPrimitiveFromHandle(RayGE_ResourceHandle handle);
