/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_TYPES_H
#define QUARTZ_SHADERS_TYPES_H

#ifdef __cplusplus

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

#endif // __cplusplus

struct Material // std430 layout
{
    vec4 albedo;
};

struct Attributes // std430 layout
{
    vec4 position;
    vec4 normal;
    vec4 tangent;
    vec4 bitangent;
    vec4 texcoord;
};

struct Face // std430 layout
{
    uint vertices[3];
};

struct EntityInstance // std430 layout
{
    uint materialIndex;
    uint geometryIndex;
};

struct RenderParameters // std430 layout
{
    vec4 cameraPositionAndAspect;
    vec4 cameraUpVectorAndTanHalfFOV;
    vec4 cameraRightVector;
    vec4 cameraForwardVector;
    vec4 skyColor;
    uint params[4];
};

#ifdef __cplusplus
} // Vulkan
} // Qt3DRaytrace
#endif // __cplusplus

#endif // QUARTZ_SHADERS_TYPES_H
