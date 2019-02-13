/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <cstdint>
#include <QtGlobal>

namespace Qt3DRaytrace {
namespace Vulkan {

namespace glsl {

template<typename T, size_t N>
struct vec
{
    T &operator[](int index)
    {
        Q_ASSERT(index >= 0 && index < N);
        return data[index];
    }
    const T &operator[](int index) const
    {
        Q_ASSERT(index >= 0 && index < N);
        return data[index];
    }
    T data[N];
};

} // glsl

using uint = unsigned int;
using vec2 = glsl::vec<float, 2>;
using vec3 = glsl::vec<float, 3>;
using vec4 = glsl::vec<float, 4>;

#include <renderers/vulkan/shaders/lib/shared.glsl>
#include <renderers/vulkan/shaders/lib/bindings.glsl>

} // Vulkan
} // Qt3DRaytrace
