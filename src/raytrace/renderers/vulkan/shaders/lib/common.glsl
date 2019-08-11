/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_COMMON_H
#define QUARTZ_SHADERS_COMMON_H

#include "bindings.glsl"
#include "shared.glsl"

const float PI       = 3.141592;
const float HalfPI   = 0.5 * PI;
const float TwoPI    = 2.0 * PI;
const float InvPI    = 1.0 / PI;
const float InvTwoPI = 1.0 / TwoPI;

const float Epsilon  = 0.0001;
const float Infinity = 1000000.0;

const float MinRoughness = 0.02;
const float MinTerminationThreshold = 0.05;

#include "sampling.glsl"

struct PathTracePayload {
    vec3 L; // Radiance
    vec3 T; // Path throughput
    RNG rng;
    uint depth;
};

struct Triangle {
    Attributes v1;
    Attributes v2;
    Attributes v3;
};

struct TangentBasis {
    vec3 T, N, B;
};

struct DifferentialSurface {
    TangentBasis basis;
    vec3 albedo;
	vec3 reflectance;
    float roughness;
    float metalness;
	float alpha, alpha2;
	float swSpecular;
};

float maxcomp(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

bool isblack(vec3 v)
{
    return dot(v, v) < Epsilon;
}

float luminance(vec3 color)
{
	return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

float pow2(float x)
{
    return x * x;
}

float pow5(float x)
{
    return (x * x) * (x * x) * x;
}

vec2 spherical(vec3 w)
{
    return vec2(
        atan(w.z, w.x) * InvTwoPI,
        acos(w.y) * InvPI
    );
}

vec2 skyuv(vec3 w)
{
    vec2 s = spherical(w);
    return vec2(s.x, 1.0 - s.y);
}

#endif // QUARTZ_SHADERS_COMMON_H
