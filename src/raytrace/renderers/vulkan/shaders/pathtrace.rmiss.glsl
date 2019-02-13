#version 460
/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#extension GL_NV_ray_tracing : require
#extension GL_GOOGLE_include_directive : require

#include "lib/common.glsl"
#include "lib/rayhit.glsl"

layout(push_constant) uniform RenderParametersBlock
{
    RenderParameters renderParams;
};

rayPayloadInNV RayPayload payload;

void main()
{
    payload.L = step(payload.depth, 0) * renderParams.skyRadiance.rgb;
}
