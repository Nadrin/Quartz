#version 460
/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_NV_ray_tracing : require

#include "lib/common.glsl"
#include "lib/rayhit.glsl"
#include "lib/sampling.glsl"
#include "lib/resources.glsl"

layout(set=DS_Render, binding=Binding_TLAS) uniform accelerationStructureNV scene;

layout(push_constant) uniform RenderParametersBlock
{
    RenderParameters renderParams;
};

rayPayloadInNV RayPayload payload;
hitAttributeNV vec2 hitBarycentrics;

layout(location=1) rayPayloadNV vec3 directLi;
layout(location=2) rayPayloadNV RayPayload indirectPayload;

vec3 sampleBRDF(Material material, vec3 wo, vec3 wi)
{
    return material.albedo.rgb * InvPI;
}

vec3 directLighting(vec3 p, vec3 wo, TangentBasis basis, Material material)
{
    vec3 wi = sampleHemisphereCosine(nextVec2(payload.rng));
    float cosTheta = cosThetaTangent(wi);
    float pdf = pdfHemisphereCosine(cosTheta);

    vec3 brdf = sampleBRDF(material, wo, wi);

    Ray ray;
    ray.p = p;
    ray.d = tangentToWorld(basis, wi);
    traceNV(scene, gl_RayFlagsNoneNV, 0xFF, Shader_SampleLightHit, 1, Shader_SampleLightMiss, ray.p, Epsilon, ray.d, Infinity, 1);

    vec3 Li = (directLi * brdf * cosTheta) / pdf;
    return payload.T * Li;
}

vec3 indirectLighting(vec3 p, vec3 wo, TangentBasis basis, Material material, uint minDepth)
{
    vec3 wi = sampleHemisphereCosine(nextVec2(payload.rng));
    float cosTheta = cosThetaTangent(wi);
    float pdf = pdfHemisphereCosine(cosTheta);

    vec3 brdf = sampleBRDF(material, wo, wi);
    vec3 pathThroughput = payload.T * (brdf * cosTheta) / pdf;

    if(payload.depth > minDepth) {
        float terminationThreshold = max(MinTerminationThreshold, 1.0 - pathThroughput.g);
        if(nextFloat(payload.rng) < terminationThreshold) {
            return vec3(0.0);
        }
        pathThroughput /= 1.0 - terminationThreshold;
    }

    indirectPayload.L     = vec3(0.0);
    indirectPayload.T     = pathThroughput;
    indirectPayload.rng   = payload.rng;
    indirectPayload.depth = payload.depth + 1;

    Ray ray;
    ray.p = p;
    ray.d = tangentToWorld(basis, wi);
    traceNV(scene, gl_RayFlagsNoneNV, 0xFF, Shader_PathTraceHit, 1, Shader_PathTraceMiss, ray.p, Epsilon, ray.d, Infinity, 2);
    return indirectPayload.L;
}

void main()
{
    const uint kMinDepth = renderParams.settings[RenderSetting_MinDepth];
    const uint kMaxDepth = renderParams.settings[RenderSetting_MaxDepth];

    EntityInstance instance = fetchInstance();
    Triangle triangle = fetchTriangle();
    Material material = fetchMaterial();

    float hitT;
    Ray hitRay = rayGetHit(hitT);

    vec3 p  = hitRay.p + hitT * hitRay.d;
    vec3 wo = -hitRay.d;

    TangentBasis basis = getTangentBasis(triangle, hitBarycentrics, instance.basisObjectToWorld);

    payload.L += step(payload.depth, 0) * material.emission.rgb;
    payload.L += directLighting(p, wo, basis, material);
    if(payload.depth + 1 <= kMaxDepth) {
        payload.L += indirectLighting(p, wo, basis, material, kMinDepth);
    }
}
