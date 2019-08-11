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
float D_ggx(float alpha2, float cos_wh)
{
    return alpha2 / (PI * pow2(pow2(cos_wh) * (alpha2 - 1.0) + 1.0));
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
vec3 sampleD_ggx(vec2 u, float alpha2)
{
    float phi    = TwoPI * u.x;
    float cos_wh = sqrt((1.0 - u.y) / (1.0 + (alpha2 - 1.0) * u.y));
    float sin_wh = sqrt(1.0 - pow2(cos_wh));
    return vec3(
        sin_wh * cos(phi),
        sin_wh * sin(phi),
        cos_wh
    );
}

// GGX NDF sample pdf (with respect to solid angle).
float pdfD_ggx(float alpha2, float cos_wh)
{
    return D_ggx(alpha2, cos_wh) * cos_wh;
}

void initializeSurfaceBSDF(inout DifferentialSurface surface)
{
    // Fresnel reflectance at normal incidence (for metals use albedo color).
	surface.reflectance = mix(kF0_dielectric, surface.albedo, surface.metalness);

	// Specular BRDF coefficients.
	surface.alpha  = pow2(surface.roughness);
	surface.alpha2 = pow2(surface.alpha);

	// Sampling weights.
	float weightDiffuse  = mix(luminance(surface.albedo), 0.0, surface.metalness);
	float weightSpecular = luminance(surface.reflectance);
	surface.swSpecular   = min(1.0, weightSpecular / (weightDiffuse + weightSpecular));
}

vec3 evaluateBSDF(DifferentialSurface surface, vec3 wo, vec3 wi, vec3 wh)
{
    float cos_wo = cosThetaTangent(wo);
    float cos_wi = cosThetaTangent(wi);
    float cos_wh = cosThetaTangent(wh);
	
    float D = D_ggx(surface.alpha2, cos_wh);
    float G = G_schlick_ggx(surface.alpha, cos_wo, cos_wi);
    vec3  F = F_schlick(surface.reflectance, wo, wh);

    // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
    // Metals on the other hand either reflect or absorb energy so diffuse contribution is always zero.
    // To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), surface.metalness);
    vec3 diffuse = kd * surface.albedo * InvPI;

    // Cook-Torrance specular microfacet BRDF.
    // cos_wi & cos_wo factors in the normalization term cancel out
    // with respective factors in the geometric shadowing function (G).
    vec3 specular = (D * F * G) * 0.25; /* / (cos_wi * cos_wo) */

    return diffuse + specular;
}

float pdfBSDF(DifferentialSurface surface, vec3 wo, vec3 wi, vec3 wh)
{
    // Specular pdf normalization term is due to change of variables.
    // We are integrating wi but GGX NDF describes distribution of microfacets in terms of wh.
    float pdfDiffuse  = pdfHemisphereCosine(cosThetaTangent(wi));
    float pdfSpecular = pdfD_ggx(surface.alpha2, cosThetaTangent(wh)) / max(Epsilon, 4.0 * dot(wi, wh));
    return mix(pdfDiffuse, pdfSpecular, surface.swSpecular);
}

vec3 sampleBSDF(DifferentialSurface surface, inout RNG rng, vec3 wo, out vec3 wi, out float pdf)
{
    vec3 wh;

    vec3 u = nextVec3(rng);

	// Sample either specular or diffuse BRDF based on sampling weights.
    if(u.z < surface.swSpecular) {
        // TODO: Sample from both D & G terms (aligned *and* visible microfacets).
        wh = sampleD_ggx(u.xy, surface.alpha2);
        wi = -reflect(wo, wh);
    }
    else {
        wi = sampleHemisphereCosine(u.xy);
        wh = normalize(wi + wo);
    }

    pdf = pdfBSDF(surface, wo, wi, wh);
    return evaluateBSDF(surface, wo, wi, wh);
}

#endif // QUARTZ_SHADERS_BRDF_H
