/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <QtGlobal>

#include <QVector3D>
#include <QVector2D>
#include <QVector4D>
#include <QMatrix3x3>
#include <QMatrix4x4>

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

struct vec2 : glsl::vec<float, 2>
{
    vec2() = default;
    vec2(const QVector2D &v)
    {
        data[0] = v.x();
        data[1] = v.y();
    }
};

struct vec3 : glsl::vec<float, 4>
{
    vec3() = default;
    vec3(const QVector3D &v)
    {
        data[0] = v.x();
        data[1] = v.y();
        data[2] = v.z();
    }
};

struct vec4 : glsl::vec<float, 4>
{
    vec4() = default;
    vec4(const QVector4D &v)
    {
        data[0] = v.x();
        data[1] = v.y();
        data[2] = v.z();
        data[3] = v.w();
    }
};

struct mat3x3 : glsl::vec<float, 12>
{
    mat3x3() = default;
    mat3x3(const QMatrix3x3 &m)
    {
        const float *md = m.constData();
        data[0] = md[0]; data[1] = md[1]; data[2]  = md[2];
        data[4] = md[3]; data[5] = md[4]; data[6]  = md[5];
        data[8] = md[6]; data[9] = md[7]; data[10] = md[8];
    }
};
using mat3 = mat3x3;

struct mat4x4 : glsl::vec<float, 16>
{
    mat4x4() = default;
    mat4x4(const QMatrix4x4 &m)
    {
        std::memcpy(data, m.constData(), sizeof(*this));
    }
};
using mat4 = mat4x4;

struct mat3x4 : glsl::vec<float, 12>
{
    mat3x4() = default;
    mat3x4(const QMatrix3x4 &m)
    {
        std::memcpy(data, m.constData(), sizeof(*this));
    }
};

#include <renderers/vulkan/shaders/lib/shared.glsl>
#include <renderers/vulkan/shaders/lib/bindings.glsl>

} // Vulkan
} // Qt3DRaytrace
