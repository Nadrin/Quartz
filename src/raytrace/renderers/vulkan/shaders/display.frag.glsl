#version 460
/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#extension GL_GOOGLE_include_directive : require

#include "lib/common.glsl"

layout(location=0) in vec2 uv;
layout(location=0) out vec4 outColor;

layout(set=DS_Display, binding=Binding_DisplayBuffer) uniform sampler2D displayBuffer;

layout(push_constant) uniform DisplayParametersBlock
{
    DisplayParameters displayParams;
};

const float MaxLuminance = 1.0;

void main()
{
    vec3 linearColor = texture(displayBuffer, uv).rgb * displayParams.exposure;

    // Reinhard tonemapping operator.
    // see: "Photographic Tone Reproduction for Digital Images", eq. 4
    float luminance = dot(linearColor, vec3(0.2126, 0.7152, 0.0722));
    float mappedLuminance = (luminance * (1.0 + luminance/(MaxLuminance*MaxLuminance))) / (1.0 + luminance);

    // Scale color by ratio of average luminances.
    vec3 mappedColor = (mappedLuminance / luminance) * linearColor;

    // Gamma correction.
    outColor = vec4(pow(mappedColor, vec3(displayParams.invGamma)), 1.0);
}
