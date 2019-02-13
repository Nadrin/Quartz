#version 460
/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#extension GL_GOOGLE_include_directive : require
#extension GL_NV_ray_tracing : require

#include "lib/common.glsl"
#include "lib/raygen.glsl"
#include "lib/sampling.glsl"

layout(push_constant) uniform RenderParametersBlock
{
    RenderParameters renderParams;
};

layout(set=DS_Render, binding=Binding_TLAS) uniform accelerationStructureNV scene;
layout(set=DS_Render, binding=Binding_RenderBuffer, rgba16f) restrict writeonly uniform image2D renderBuffer;
layout(set=DS_Render, binding=Binding_PrevRenderBuffer, rgba16f) restrict readonly uniform image2D prevRenderBuffer;

layout(location=0) rayPayloadNV RayPayload payload;

Ray generateCameraRay(vec2 pixelLocation)
{
    const vec3 upVector = renderParams.cameraUpVectorAndTanHalfFOV.xyz;
    const vec3 rightVector = renderParams.cameraRightVector.xyz;
    const vec3 forwardVector = renderParams.cameraForwardVector.xyz;
    const float aspect = renderParams.cameraPositionAndAspect.w;
    const float tanHalfFOV = renderParams.cameraUpVectorAndTanHalfFOV.w;

    float tx = (2.0 * pixelLocation.x - 1.0) * tanHalfFOV * aspect;
    float ty = (2.0 * pixelLocation.y - 1.0) * tanHalfFOV;

    Ray ray;
    ray.p = renderParams.cameraPositionAndAspect.xyz;
    ray.d = normalize(rightVector * tx + upVector * ty + forwardVector);
    return ray;
}

void main()
{
    const uint kFrameNumber = renderParams.frame[FrameParam_FrameNumber];

    vec2 pixelSize = vec2(1.0) / vec2(gl_LaunchSizeNV.xy);
    vec2 pixelLocation = vec2(gl_LaunchIDNV.xy) * pixelSize;

    vec3 prevColor = imageLoad(prevRenderBuffer, ivec2(gl_LaunchIDNV)).rgb;

    payload.rng   = rngInit(gl_LaunchIDNV.xy, kFrameNumber);
    payload.L     = vec3(0.0);
    payload.T     = vec3(1.0);
    payload.depth = 0;

    vec2 delta = sampleRectangle(nextVec2(payload.rng), -0.5 * pixelSize, 0.5 * pixelSize);
    Ray ray = generateCameraRay(pixelLocation + delta);
    traceNV(scene, gl_RayFlagsNoneNV, 0xFF, Shader_PathTraceHit, 1, Shader_PathTraceMiss, ray.p, 0.0, ray.d, Infinity, 0);

    vec3 currentColor = prevColor + (payload.L - prevColor) / float(kFrameNumber);
    imageStore(renderBuffer, ivec2(gl_LaunchIDNV), vec4(currentColor, 1.0));
}
