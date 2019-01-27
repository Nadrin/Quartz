/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/vkresources.h>

namespace Qt3DRaytrace {
namespace Vulkan {

struct Geometry
{
    Buffer attributes;
    Buffer indices;
    AccelerationStructure blas;
    uint64_t blasHandle = 0;
    uint32_t numVertices = 0;
    uint32_t numIndices = 0;
};

} // Vulkan
} // Qt3DRaytrace
