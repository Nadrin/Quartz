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

struct Attributes // std430 layout
{
    float position[4];
    float normal[4];
    float tangent[4];
    float bitangent[4];
    float texcoord[4];
};

struct Geometry
{
    Buffer attributes;
    Buffer indices;
    AccelerationStructure blas;
    uint64_t blasHandle = 0;
    uint32_t numVertices = 0;
    uint32_t numIndices = 0;
};

struct GeometryInstance
{
    float transform[12];
    uint32_t instanceCustomIndex : 24;
    uint32_t mask : 8;
    uint32_t instanceOffset : 24;
    uint32_t flags : 8;
    uint64_t blasHandle;
};

} // Vulkan
} // Qt3DRaytrace
