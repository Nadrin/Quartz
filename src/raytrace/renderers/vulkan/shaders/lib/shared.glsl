/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_TYPES_H
#define QUARTZ_SHADERS_TYPES_H

struct Material
{
    vec3 albedo;
    vec3 emission;
};

struct Emitter
{
    vec3 radiance;
    mat4x3 transform;
    uint geometryIndex;
    uint geometryFaceCount;
    float _padding[2];
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

struct EntityInstance
{
    uint materialIndex;
    uint geometryIndex;
    float _padding[2];
    mat3 basisTransform;
};

struct RenderParameters
{
    uint minDepth;
    uint maxDepth;
    uint frameNumber;
    uint numEmitters;
    uint numPrimarySamples;
    uint numSecondarySamples;
    uint _reserved[2];
    vec4 cameraPositionAndAspect;
    vec4 cameraUpVectorAndTanHalfFOV;
    vec4 cameraRightVector;
    vec4 cameraForwardVector;
};

struct DisplayParameters
{
    float invGamma;
    float exposure;
    float _padding[2];
};

#endif // QUARTZ_SHADERS_TYPES_H
