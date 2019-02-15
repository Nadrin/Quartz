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
    RenderParameters params;
};

rayPayloadInNV RayPayload payload;
hitAttributeNV vec2 hitBarycentrics;

layout(location=1) rayPayloadNV float pVisibility;
layout(location=2) rayPayloadNV vec3 pEmission;
layout(location=3) rayPayloadNV RayPayload pIndirect;

// MIS power heuristic for two samples taken from two different distributions
// pdfA and pdfB. The Beta parameter is assumed to be 2.
float powerHeuristic(float pdfA, float pdfB)
{
    float f = pdfA * pdfA;
    float g = pdfB * pdfB;
    return f / (f + g);
}

vec3 sampleBRDF(Material material, vec3 wo, vec3 wi)
{
    return material.albedo.rgb * InvPI;
}

vec3 sampleEmitterLi(vec3 p, TangentBasis basis, out vec3 wiWorld, out vec3 wiTangent, out float pdf)
{
    const uint emitterIndex = nextUInt(payload.rng, params.numEmitters);
    const Emitter emitter = fetchEmitter(emitterIndex);

    if(emitterIndex == 0) {
        // Sky emitter.
        wiTangent = sampleHemisphereCosine(nextVec2(payload.rng));
        wiWorld   = tangentToWorld(basis, wiTangent);
        pdf       = pdfHemisphereCosine(cosThetaTangent(wiTangent));
    }
    else if(emitter.geometryIndex == ~0u) {
        // Distant light emitter.
        wiWorld   = -emitter.transform[0].xyz;
        //wiWorld = vec3(0.0, 1.0, 0.0);
        wiTangent = worldToTangent(basis, wiWorld);
        pdf       = 0.0;
    }
    else {
        // Area emitter.
        // TODO: Implement
    }

    traceNV(scene, gl_RayFlagsTerminateOnFirstHitNV, 0xFF, Shader_QueryVisibilityHit, 1, Shader_QueryVisibilityMiss, p, Epsilon, wiWorld, Infinity, 1);
    return emitter.radiance * pVisibility;
}

vec3 sampleScatteringDirection(vec3 p, Material material, out float pdf)
{
    vec3 wiTangent = sampleHemisphereCosine(nextVec2(payload.rng));
    float cosTheta = cosThetaTangent(wiTangent);
    pdf = pdfHemisphereCosine(cosTheta);
    return wiTangent;
}

vec3 sampleScatteringLi(vec3 p, Material material, TangentBasis basis, out vec3 wiTangent, out float pdf)
{
    wiTangent = sampleScatteringDirection(p, material, pdf);
    if(pdf < Epsilon) {
        return vec3(0.0);
    }

    vec3 wiWorld = tangentToWorld(basis, wiTangent);
    traceNV(scene, gl_RayFlagsNoneNV, 0xFF, Shader_QueryEmissionHit, 1, Shader_QueryEmissionMiss, p, Epsilon, wiWorld, Infinity, 2);
    return pEmission;
}

vec3 directLighting(vec3 p, vec3 wo, TangentBasis basis, Material material)
{
    vec3 L = vec3(0.0);

    vec3  emitterWiWorld, emitterWiTangent;
    float emitterPdf;
    vec3  emitterLi = sampleEmitterLi(p, basis, emitterWiWorld, emitterWiTangent, emitterPdf);
    if(!isblack(emitterLi)) {
        vec3 wi = emitterWiTangent;
        float cosTheta = cosThetaTangent(wi);
        vec3 brdf = sampleBRDF(material, wo, wi);
        if(emitterPdf != 0.0) {
            // Area emitter: Add contribution with MIS heuristic.
            float scatteringPdf = pdfHemisphereCosine(cosTheta);
            float weight = powerHeuristic(emitterPdf, scatteringPdf);
            L += (emitterLi * brdf * cosTheta * weight) / emitterPdf;
        }
        else {
            // Delta-distribution emitter: add contribution directly.
            L += emitterLi * brdf * cosTheta;
        }
    }

    if(emitterPdf != 0.0) {
        vec3  scatteringWiTangent;
        float scatteringPdf;
        vec3  scatteringLi = sampleScatteringLi(p, material, basis, scatteringWiTangent, scatteringPdf);
        if(!isblack(scatteringLi)) {
            vec3 wi = scatteringWiTangent;
            float cosTheta = cosThetaTangent(wi);
            vec3 brdf = sampleBRDF(material, wo, wi);
            float weight = powerHeuristic(scatteringPdf, emitterPdf);
            L += (scatteringLi * brdf * cosTheta * weight) / scatteringPdf;
        }
    }
    return payload.T * params.numEmitters * L;
}

vec3 indirectLighting(vec3 p, vec3 wo, TangentBasis basis, Material material, uint minDepth)
{
    float pdf;
    vec3 wi = sampleScatteringDirection(p, material, pdf);
    if(pdf < Epsilon) {
        return vec3(0.0);
    }

    float cosTheta = cosThetaTangent(wi);
    vec3 brdf = sampleBRDF(material, wo, wi);
    vec3 pathThroughput = payload.T * (brdf * cosTheta) / pdf;

    if(payload.depth > minDepth) {
        float terminationThreshold = max(MinTerminationThreshold, 1.0 - maxcomp3(pathThroughput));
        if(nextFloat(payload.rng) < terminationThreshold) {
            return vec3(0.0);
        }
        pathThroughput /= 1.0 - terminationThreshold;
    }

    pIndirect.T     = pathThroughput;
    pIndirect.rng   = payload.rng;
    pIndirect.depth = payload.depth + 1;

    vec3 wiWorld = tangentToWorld(basis, wi);
    traceNV(scene, gl_RayFlagsNoneNV, 0xFF, Shader_PathTraceHit, 1, Shader_PathTraceMiss, p, Epsilon, wiWorld, Infinity, 3);
    return pIndirect.L;
}

void main()
{
    EntityInstance instance = fetchInstance();
    Triangle triangle = fetchTriangle();
    Material material = fetchMaterial();

    float hitT;
    Ray hitRay = rayGetHit(hitT);

    vec3 p  = hitRay.p + hitT * hitRay.d;
    vec3 wo = -hitRay.d;

    TangentBasis basis = getTangentBasis(triangle, hitBarycentrics, instance.basisTransform);

    payload.L  = step(payload.depth, 0) * material.emission.rgb;
    payload.L += directLighting(p, wo, basis, material);
    if(payload.depth + 1 <= params.maxDepth) {
        payload.L += indirectLighting(p, wo, basis, material, params.minDepth);
    }
}
