/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_TYPES_H
#define QUARTZ_SHADERS_TYPES_H

struct Material
{
    vec4 albedo; // +roughness
    vec4 emission; // +metalness
    uint albedoTexture;
    uint roughnessTexture;
    uint metalnessTexture;
    float _padding[1];
};

struct Emitter
{
    uint instanceIndex;
    uint geometryIndex;
    uint textureIndex;
    float intensity;
    vec3 radiance;
    vec3 direction;
};

struct Attributes
{
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec2 texcoord;
    float _padding[2];
};

struct Face
{
    uint vertices[3];
};

// TODO: Switch to mat3x4 for transform to save space.
struct EntityInstance
{
    uint materialIndex;
    uint geometryIndex;
    uint geometryNumFaces;
    float _padding[1];
    mat4x4 transform;
    mat3x3 basisTransform;
};

struct RenderParameters
{
    uint minDepth;
    uint maxDepth;
    uint frameNumber;
    uint numEmitters;
    uint numPrimarySamples;
    uint numSecondarySamples;
    float directRadianceClamp;
    float indirectRadianceClamp;
    vec4 cameraPositionAspect;
    vec4 cameraUpVectorTanHalfFOV;
    vec4 cameraRightVectorLensR;
    vec4 cameraForwardVectorLensF;
};

struct DisplayParameters
{
    float invGamma;
    float exposure;
    float _padding[2];
};

#endif // QUARTZ_SHADERS_TYPES_H
