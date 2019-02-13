/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_RAYHIT_H
#define QUARTZ_SHADERS_RAYHIT_H

#include "raygen.glsl"

struct Triangle {
    Attributes v1;
    Attributes v2;
    Attributes v3;
};

struct TangentBasis {
    vec3 T, N, B;
};

Ray rayGetHit(out float t)
{
    Ray ray;
    ray.p = gl_WorldRayOriginNV;
    ray.d = gl_WorldRayDirectionNV;
    t = gl_RayTmaxNV;
    return ray;
}

vec3 blerp(vec2 t, vec3 p1, vec3 p2, vec3 p3)
{
    return (1.0 - t.x - t.y) * p1 + t.x * p2 + t.y * p3;
}

vec3 getNormal(Triangle triangle, vec2 hit)
{
    return normalize(blerp(hit, triangle.v1.normal.xyz, triangle.v2.normal.xyz, triangle.v3.normal.xyz));
}

vec3 getTangent(Triangle triangle, vec2 hit)
{
    return normalize(blerp(hit, triangle.v1.tangent.xyz, triangle.v2.tangent.xyz, triangle.v3.tangent.xyz));
}

TangentBasis getTangentBasis(Triangle triangle, vec2 hit)
{
    TangentBasis basis;
    basis.N = getNormal(triangle, hit);
    basis.T = getTangent(triangle, hit);
    basis.B = cross(basis.N, basis.T);
    return basis;
}

vec3 tangentToWorld(TangentBasis basis, vec3 v)
{
    return basis.T * v.x + basis.B * v.y + basis.N * v.z;
}

vec3 worldToTangent(TangentBasis basis, vec3 v)
{
    return vec3(dot(basis.T, v), dot(basis.B, v), dot(basis.N, v));
}

float cosThetaTangent(vec3 v)
{
    return max(v.z, 0.0);
}

#endif // QUARTZ_SHADERS_RAYHIT_H