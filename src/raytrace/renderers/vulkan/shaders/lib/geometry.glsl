/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_RAYHIT_H
#define QUARTZ_SHADERS_RAYHIT_H

#include "common.glsl"

vec2 blerp(vec2 b, vec2 p1, vec2 p2, vec2 p3)
{
    return (1.0 - b.x - b.y) * p1 + b.x * p2 + b.y * p3;
}

vec3 blerp(vec2 b, vec3 p1, vec3 p2, vec3 p3)
{
    return (1.0 - b.x - b.y) * p1 + b.x * p2 + b.y * p3;
}

vec3 getPosition(Triangle triangle, vec2 b)
{
    return blerp(b, triangle.v1.position, triangle.v2.position, triangle.v3.position);
}

vec3 getPositionWorld(Triangle triangle, mat4x4 objectToWorld, vec2 b)
{
    return vec3(objectToWorld * vec4(getPosition(triangle, b), 1.0));
}

vec3 getNormal(Triangle triangle, vec2 b)
{
    return blerp(b, triangle.v1.normal, triangle.v2.normal, triangle.v3.normal);
}

vec3 getNormalWorld(Triangle triangle, mat3x3 basisObjectToWorld, vec2 b)
{
    return normalize(basisObjectToWorld * getNormal(triangle, b));
}

vec3 getTangent(Triangle triangle, vec2 b)
{
    return blerp(b, triangle.v1.tangent, triangle.v2.tangent, triangle.v3.tangent);
}

vec3 getTangentWorld(Triangle triangle, mat3x3 basisObjectToWorld, vec2 b)
{
    return normalize(basisObjectToWorld * getTangent(triangle, b));
}

vec2 getTexCoord(Triangle triangle, vec2 b)
{
    return blerp(b, triangle.v1.texcoord, triangle.v2.texcoord, triangle.v3.texcoord);
}

TangentBasis getTangentBasis(Triangle triangle, mat3x3 basisObjectToWorld, vec2 b)
{
    TangentBasis basis;
    basis.N = getNormalWorld(triangle, basisObjectToWorld, b);
    basis.T = getTangentWorld(triangle, basisObjectToWorld, b);
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

float cosThetaWorld(vec3 v, vec3 N)
{
    return max(dot(v, N), 0.0);
}

float cosThetaTangent(vec3 v)
{
    return max(v.z, 0.0);
}

#endif // QUARTZ_SHADERS_RAYHIT_H