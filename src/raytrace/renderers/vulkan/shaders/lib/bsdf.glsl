/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_BRDF_H
#define QUARTZ_SHADERS_BRDF_H

#include "common.glsl"

// Average fresnel factor at angle of incidence for dielectrics.
const vec3 kF0_dielectric = vec3(0.04);

// Schlick's approximation of the Fresnel function.
vec3 F_schlick(vec3 F0, vec3 wo, vec3 wh)
{
    return F0 + (vec3(1.0) - F0) * pow5(1.0 - max(0.0, dot(wo, wh)));
}

// Trowbridge-Reitz (GGX) normal distribution function.
float D_ggx(float alphaSqr, float cos_wh)
{
    return alphaSqr / (PI * pow2(pow2(cos_wh) * (alphaSqr - 1.0) + 1.0));
}

// Single term for separable Schlick-GGX below.
float G1_shclick_ggx(float k, float cosTheta)
{
    return cosTheta * (1.0 - k) + k;
}

// Schlick's approximation for Beckmann geometric shadowing function using Smith's method.
// Uses Brian Karis' remapping of k=alpha/2 to better match Smith's model for GGX.
float G_schlick_ggx(float alpha, float cos_wo, float cos_wi)
{
    float k = 0.5 * alpha;
    // Numerator cancels out with cos_wi & cos_wo in specular BRDF normalization factor.
    return 1.0 /* cos_wi * cos_wo */ / (G1_shclick_ggx(k, cos_wi) * G1_shclick_ggx(k, cos_wo));
}

// Sample half-angle directions from GGX normal distribution function.
vec3 sampleD_ggx(vec2 u, float alphaSqr)
{
    float phi    = TwoPI * u.x;
    float cos_wh = sqrt((1.0 - u.y) / (1.0 + (alphaSqr - 1.0) * u.y));
    float sin_wh = sqrt(1.0 - pow2(cos_wh));
    return vec3(
        sin_wh * cos(phi),
        sin_wh * sin(phi),
        cos_wh
    );
}

// GGX NDF sample pdf (with respect to solid angle).
float pdfD_ggx(float alphaSqr, float cos_wh)
{
    return D_ggx(alphaSqr, cos_wh) * cos_wh;
}

vec3 evaluateBSDF(vec3 wo, vec3 wi, vec3 wh, vec3 albedo, float alpha, float metalness)
{
    float cos_wo = cosThetaTangent(wo);
    float cos_wi = cosThetaTangent(wi);
    float cos_wh = cosThetaTangent(wh);

    // Fresnel reflectance at normal incidence (for metals use albedo color).
    vec3 F0 = mix(kF0_dielectric, albedo, metalness);
    
    float D = D_ggx(pow2(alpha), cos_wh);
    float G = G_schlick_ggx(alpha, cos_wo, cos_wi);
    vec3  F = F_schlick(F0, wo, wh);

    // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
    // Metals on the other hand either reflect or absorb energy so diffuse contribution is always zero.
    // To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);
    vec3 diffuse = kd * albedo * InvPI;

    // Cook-Torrance specular microfacet BRDF.
    // cos_wi & cos_wo factors in the normalization term cancel out
    // with respective factors in the geometric shadowing function (G).
    vec3 specular = (D * F * G) * 0.25; /* / (cos_wi * cos_wo) */

    return diffuse + specular;
}

vec3 evaluateBSDF(Material material, vec3 wo, vec3 wi, vec3 wh)
{
    const float roughness = material.albedo.a;
    const float metalness = material.emission.a;
    return evaluateBSDF(wo, wi, wh, material.albedo.rgb, pow2(roughness), metalness);
}

float pdfBSDF(vec3 wo, vec3 wi, vec3 wh, float alphaSqr)
{
    // Specular pdf normalization term is due to change of variables.
    // We are integrating wi but GGX NDF describes distribution of microfacets in terms of wh.
    float pdfDiffuse  = pdfHemisphereCosine(cosThetaTangent(wi));
    float pdfSpecular = pdfD_ggx(alphaSqr, cosThetaTangent(wh)) / (4.0 * dot(wi, wh));
    return mix(pdfDiffuse, pdfSpecular, 0.5);
}

float pdfBSDF(Material material, vec3 wo, vec3 wi, vec3 wh)
{
    const float roughness = material.albedo.a;
    float alphaSqr = pow2(pow2(roughness));
    return pdfBSDF(wo, wi, wh, alphaSqr);
}

vec3 sampleBSDF(Material material, inout RNG rng, vec3 wo, out vec3 wi, out float pdf)
{
    const float roughness = material.albedo.a;
    const float metalness = material.emission.a;

    float alpha = pow2(roughness);
    float alphaSqr = pow2(alpha);

    vec3 wh;

    vec2 u = nextVec2(rng);
    if(u.x < 0.5) {
        wi = sampleHemisphereCosine(vec2(2.0 * u.x, u.y));
        wh = normalize(wi + wo);
    }
    else {
        // TODO: Sample from both D & G terms (aligned *and* visible microfacets).
        wh = sampleD_ggx(vec2(2.0 * (u.x-0.5), u.y), alphaSqr);
        wi = -reflect(wo, wh);
    }

    pdf = pdfBSDF(wo, wi, wh, alphaSqr);
    return evaluateBSDF(wo, wi, wh, material.albedo.rgb, alpha, metalness);
}

#endif // QUARTZ_SHADERS_BRDF_H
