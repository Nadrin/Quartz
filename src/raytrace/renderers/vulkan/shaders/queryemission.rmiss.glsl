#version 460
/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

#include "lib/common.glsl"

layout(set=DS_Render, binding=Binding_Emitters, std430) readonly buffer EmitterBuffer {
    Emitter emitters[];
} emitterBuffer;

rayPayloadInNV vec3 pEmission;

void main()
{
    // Emitter #0 is always sky.
    pEmission = emitterBuffer.emitters[0].radiance;
}
