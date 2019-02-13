#version 460
/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

#include "lib/common.glsl"

layout(push_constant) uniform RenderParametersBlock
{
    RenderParameters renderParams;
};

rayPayloadInNV vec3 Le;

void main()
{
    Le = renderParams.skyRadiance.rgb;
}
