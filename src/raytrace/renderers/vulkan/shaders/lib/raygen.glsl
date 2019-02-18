/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_RAY_H
#define QUARTZ_SHADERS_RAY_H

#include "common.glsl"
#include "sampling.glsl"

struct Ray {
    vec3 p; // Origin
    vec3 d; // Direction
};

struct PathTracePayload {
    vec3 L; // Radiance
    vec3 T; // Path throughput
    RNG rng;
    uint depth;
};

#endif // QUARTZ_SHADERS_RAY_H