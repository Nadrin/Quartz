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
    RenderParameters params;
};

layout(set=DS_Render, binding=Binding_TLAS) uniform accelerationStructureNV scene;
layout(set=DS_Render, binding=Binding_RenderBuffer, rgba16f) restrict writeonly uniform image2D renderBuffer;
layout(set=DS_Render, binding=Binding_PrevRenderBuffer, rgba16f) restrict readonly uniform image2D prevRenderBuffer;

layout(location=0) rayPayloadNV RayPayload pPathTrace;

Ray generateCameraRay(vec2 pixelLocation)
{
    const vec3 upVector = params.cameraUpVectorAndTanHalfFOV.xyz;
    const vec3 rightVector = params.cameraRightVector.xyz;
    const vec3 forwardVector = params.cameraForwardVector.xyz;
    const float aspect = params.cameraPositionAndAspect.w;
    const float tanHalfFOV = params.cameraUpVectorAndTanHalfFOV.w;

    float tx = (2.0 * pixelLocation.x - 1.0) * tanHalfFOV * aspect;
    float ty = (2.0 * pixelLocation.y - 1.0) * tanHalfFOV;

    Ray ray;
    ray.p = params.cameraPositionAndAspect.xyz;
    ray.d = normalize(rightVector * tx + upVector * ty + forwardVector);
    return ray;
}

void main()
{
    vec2 pixelSize = vec2(1.0) / vec2(gl_LaunchSizeNV.xy);
    vec2 pixelLocation = vec2(gl_LaunchIDNV.xy) * pixelSize;

    vec3 prevColor = imageLoad(prevRenderBuffer, ivec2(gl_LaunchIDNV)).rgb;

    pPathTrace.rng   = rngInit(gl_LaunchIDNV.xy, params.frameNumber);
    pPathTrace.depth = 0;
    pPathTrace.T     = vec3(1.0);

    vec2 delta = sampleRectangle(nextVec2(pPathTrace.rng), -0.5 * pixelSize, 0.5 * pixelSize);
    Ray ray = generateCameraRay(pixelLocation + delta);
    traceNV(scene, gl_RayFlagsNoneNV, 0xFF, Shader_PathTraceHit, 1, Shader_PathTraceMiss, ray.p, 0.0, ray.d, Infinity, 0);

    vec3 currentColor = prevColor + (pPathTrace.L - prevColor) / float(params.frameNumber);
    imageStore(renderBuffer, ivec2(gl_LaunchIDNV), vec4(currentColor, 1.0));
}
