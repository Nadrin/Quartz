/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_COMMON_H
#define QUARTZ_SHADERS_COMMON_H

#include "bindings.glsl"
#include "shared.glsl"

const float PI     = 3.141592;
const float HalfPI = 0.5 * PI;
const float TwoPI  = 2.0 * PI;
const float InvPI  = 1.0 / PI;

const float Epsilon  = 0.0001;
const float Infinity = 1000000.0;

const float MinTerminationThreshold = 0.05;

float maxcomp3(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

bool isblack(vec3 v)
{
    return dot(v, v) < Epsilon;
}

#endif // QUARTZ_SHADERS_COMMON_H
