/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_SAMPLING_H
#define QUARTZ_SHADERS_SAMPLING_H

#include "xoroshiro64s.glsl"

float nextFloat(inout RNG rng)
{
    uint u = 0x3f800000 | (rngNext(rng) >> 9);
    return uintBitsToFloat(u) - 1.0;
}

uint nextUInt(inout RNG rng, uint nmax)
{
    float f = nextFloat(rng);
    return uint(floor(f * nmax));
}

vec2 nextVec2(inout RNG rng)
{
    return vec2(nextFloat(rng), nextFloat(rng));
}

vec3 nextVec3(inout RNG rng)
{
    return vec3(nextFloat(rng), nextFloat(rng), nextFloat(rng));
}

vec2 nextStratified(inout RNG rng, vec2 p, float delta)
{
    return vec2(
        p.x + nextFloat(rng) * delta,
        p.y + nextFloat(rng) * delta
    );
}

vec2 sampleRectangle(vec2 u, vec2 rmin, vec2 rmax)
{
    return rmin + u * (rmax - rmin);
}

vec2 sampleDisk(vec2 u)
{
    float r = sqrt(u.x);
    float theta = TwoPI * u.y;
    return r * vec2(cos(theta), sin(theta));
}

vec2 sampleDiskConcentric(vec2 u)
{
    vec2 up = 2.0 * u - vec2(1.0);
    if(up == vec2(0.0)) {
        return vec2(0.0);
    }
    else {
        float r, theta;
        if(abs(up.x) > abs(up.y)) {
            r = up.x;
            theta = 0.25 * PI * (up.y / up.x);
        }
        else {
            r = up.y;
            theta = 0.5 * PI - 0.25 * PI * (up.x / up.y);
        }
        return r * vec2(cos(theta), sin(theta));
    }
}

float pdfDisk()
{
    return InvPI;
}

vec3 sampleHemisphere(vec2 u)
{
    float phi = TwoPI * u.y;
    float sinTheta = sqrt(1.0 - u.x*u.x);
    return vec3(
        cos(phi) * sinTheta,
        sin(phi) * sinTheta,
        u.x
    );
}

float pdfHemisphere()
{
    return 0.5 * InvPI;
}

vec3 sampleHemisphereCosine(vec2 u)
{
    vec2 d = sampleDisk(u);
    return vec3(d.x, d.y, sqrt(1.0 - d.x*d.x - d.y*d.y));
}

vec3 sampleHemisphereCosineConcentric(vec2 u)
{
    vec2 d = sampleDiskConcentric(u);
    return vec3(d.x, d.y, sqrt(1.0 - d.x*d.x - d.y*d.y));
}

float pdfHemisphereCosine(float cosTheta)
{
    return cosTheta * InvPI;
}

vec2 sampleTriangle(vec2 u)
{
    float uxsqrt = sqrt(u.x);
    return vec2(1.0 - uxsqrt, u.y * uxsqrt);
}

float pdfTriangle(float area)
{
    return 1.0 / area;
}

#endif // QUARTZ_SHADERS_SAMPLING_H