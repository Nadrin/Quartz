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
#include "lib/geometry.glsl"
#include "lib/resources.glsl"
#include "lib/bsdf.glsl"

layout(set=DS_Render, binding=Binding_TLAS) uniform accelerationStructureNV scene;

layout(push_constant) uniform RenderParametersBlock
{
    RenderParameters params;
};

rayPayloadInNV PathTracePayload payload;
hitAttributeNV vec2 hitBarycentrics;

layout(location=1) rayPayloadNV float pVisibility;
layout(location=2) rayPayloadNV vec3 pEmission;
layout(location=3) rayPayloadNV PathTracePayload pIndirect;

// MIS power heuristic for two samples taken from two different distributions
// pdfA and pdfB. The Beta parameter is assumed to be 2.
float powerHeuristic(float pdfA, float pdfB)
{
    float f = pdfA * pdfA;
    float g = pdfB * pdfB;
    return f / (f + g);
}

vec3 sampleEmitterLi(vec3 p, DifferentialSurface surface, out vec3 wiWorld, out vec3 wiTangent, out float pdf)
{
    const uint emitterIndex = nextUInt(payload.rng, params.numEmitters);
    const Emitter emitter = fetchEmitter(emitterIndex);

    vec3 emitterL = emitter.radiance;
    float emitterDistance = Infinity;

    if(emitterIndex == 0) {
        // Sky emitter.
        wiTangent = sampleHemisphereCosine(nextVec2(payload.rng));
        wiWorld   = tangentToWorld(surface.basis, wiTangent);
        pdf       = pdfHemisphereCosine(cosThetaTangent(wiTangent));
        emitterL  = fetchSkyRadiance(skyuv(wiWorld));
    }
    else if(emitter.instanceIndex == ~0u) {
        // Distant light emitter.
        wiWorld   = -emitter.direction;
        wiTangent = worldToTangent(surface.basis, wiWorld);
        pdf       = 0.0;
    }
    else {
        // Area emitter.
        EntityInstance emitterInstance = fetchInstance(emitter.instanceIndex);
        uint faceIndex = nextUInt(payload.rng, emitterInstance.geometryNumFaces);
        vec2 faceBarycentrics = sampleTriangle(nextVec2(payload.rng));

        Triangle triangle = fetchTriangle(emitter.geometryIndex, faceIndex);
        vec3 p1 = vec3(emitterInstance.transform * vec4(triangle.v1.position, 1.0));
        vec3 p2 = vec3(emitterInstance.transform * vec4(triangle.v2.position, 1.0));
        vec3 p3 = vec3(emitterInstance.transform * vec4(triangle.v3.position, 1.0));

        vec3 emitterP  = blerp(faceBarycentrics, p1, p2, p3);
        vec3 emitterN  = getNormalWorld(triangle, emitterInstance.basisTransform, faceBarycentrics);
        vec3 emitterWo = p - emitterP;

        float triangleArea = 0.5 * length(cross(p2 - p1, p3 - p1));
        float distanceSqr = dot(emitterWo, emitterWo);
        if(triangleArea == 0.0 || distanceSqr == 0.0) {
            pdf = 0.0;
            return vec3(0.0);
        }

        emitterDistance = sqrt(distanceSqr);
        emitterWo /= emitterDistance;
        emitterDistance = max(0.0, emitterDistance - Epsilon);

        float cosTheta = cosThetaWorld(emitterN, emitterWo);
        if(cosTheta == 0.0) {
            pdf = 0.0;
            return vec3(0.0);
        }

        wiWorld   = -emitterWo;
        wiTangent = worldToTangent(surface.basis, wiWorld);
        pdf       = distanceSqr / (cosTheta * triangleArea);
    }

    traceNV(scene, gl_RayFlagsTerminateOnFirstHitNV, 0xFF, Shader_QueryVisibilityHit, 1, Shader_QueryVisibilityMiss, p, Epsilon, wiWorld, emitterDistance, 1);
    return emitterL * pVisibility;
}

vec3 sampleScatteringLi(vec3 p, DifferentialSurface surface, vec3 wo, out vec3 wi, out float pdf)
{
    vec3 brdf = sampleBSDF(surface, payload.rng, wo, wi, pdf);
    if(isblack(brdf) || pdf < Epsilon) {
        return vec3(0.0);
    }

    vec3 wiWorld = tangentToWorld(surface.basis, wi);
    traceNV(scene, gl_RayFlagsNoneNV, 0xFF, Shader_QueryEmissionHit, 1, Shader_QueryEmissionMiss, p, Epsilon, wiWorld, Infinity, 2);
    return pEmission * brdf;
}

vec3 directLighting(vec3 p, vec3 wo, DifferentialSurface surface)
{
    vec3 L = vec3(0.0);

    vec3  emitterWiWorld, emitterWi;
    float emitterPdf;
    vec3  emitterLi = sampleEmitterLi(p, surface, emitterWiWorld, emitterWi, emitterPdf);
    if(!isblack(emitterLi)) {
        vec3 wi = emitterWi;
        vec3 wh = normalize(wi + wo);
        float cosTheta = cosThetaTangent(wi);
        vec3 bsdf = evaluateBSDF(surface, wo, wi, wh);
        if(emitterPdf != 0.0) {
            // Area emitter: Add contribution with MIS heuristic.
            float scatteringPdf = pdfBSDF(surface, wo, wi, wh);
            float weight = powerHeuristic(emitterPdf, scatteringPdf);
            L += (emitterLi * bsdf * cosTheta * weight) / emitterPdf;
        }
        else {
            // Delta-distribution emitter: add contribution directly.
            L += emitterLi * bsdf * cosTheta;
        }
    }
    if(emitterPdf != 0.0) {
        vec3  scatteringWi;
        float scatteringPdf;
        vec3  scatteringLi = sampleScatteringLi(p, surface, wo, scatteringWi, scatteringPdf);
        if(!isblack(scatteringLi)) {
            vec3 wi = scatteringWi;
            float cosTheta = cosThetaTangent(wi);
            float weight = powerHeuristic(scatteringPdf, emitterPdf);
            L += (scatteringLi * cosTheta * weight) / scatteringPdf;
        }
    }
    return min(payload.T * params.numEmitters * L, vec3(params.directRadianceClamp));
}

vec3 indirectLighting(vec3 p, vec3 wo, DifferentialSurface surface, uint minDepth)
{
    vec3 wi;
    float pdf;
    vec3 brdf = sampleBSDF(surface, payload.rng, wo, wi, pdf);
    if(isblack(brdf) || pdf < Epsilon) {
        return vec3(0.0);
    }

    float cosTheta = cosThetaTangent(wi);
    vec3 pathThroughput = payload.T * (brdf * cosTheta) / pdf;

    if(payload.depth > minDepth) {
        float terminationThreshold = max(MinTerminationThreshold, 1.0 - maxcomp(pathThroughput));
        if(nextFloat(payload.rng) < terminationThreshold) {
            return vec3(0.0);
        }
        pathThroughput /= 1.0 - terminationThreshold;
    }

    pIndirect.T     = pathThroughput;
    pIndirect.rng   = payload.rng;
    pIndirect.depth = payload.depth + 1;

    vec3 wiWorld = tangentToWorld(surface.basis, wi);
    traceNV(scene, gl_RayFlagsNoneNV, 0xFF, Shader_PathTraceHit, 1, Shader_PathTraceMiss, p, Epsilon, wiWorld, Infinity, 3);
    return min(pIndirect.L, vec3(params.indirectRadianceClamp));
}

void main()
{
    EntityInstance instance = fetchInstance(gl_InstanceID);
    Triangle triangle = fetchTriangle(gl_InstanceCustomIndexNV, gl_PrimitiveID);
    Material material = fetchMaterial(gl_InstanceID);
    
    vec2 uv = getTexCoord(triangle, hitBarycentrics);
    
    DifferentialSurface surface;
    surface.basis     = getTangentBasis(triangle, instance.basisTransform, hitBarycentrics);
    surface.albedo    = fetchMaterialAlbedo(material, uv);
    surface.roughness = fetchMaterialRoughness(material, uv);
    surface.metalness = fetchMaterialMetalness(material, uv);
	initializeSurfaceBSDF(surface);

    vec3 p  = gl_WorldRayOriginNV + gl_RayTmaxNV * gl_WorldRayDirectionNV;
    vec3 wo = worldToTangent(surface.basis, -gl_WorldRayDirectionNV);

    payload.L  = (payload.depth == 0) ? material.emission.rgb : vec3(0.0);
    payload.L += directLighting(p, wo, surface);
    if(payload.depth + 1 <= params.maxDepth) {
        payload.L += indirectLighting(p, wo, surface, params.minDepth);
    }
}
