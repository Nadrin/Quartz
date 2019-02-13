/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_TYPES_H
#define QUARTZ_SHADERS_TYPES_H

struct Material // std430 layout
{
    vec4 albedo;
};

struct Attributes // std430 layout
{
    vec4 position;
    vec4 normal;
    vec4 tangent;
    vec4 texcoord;
};

struct Face // std430 layout
{
    uint vertices[3];
};

struct EntityInstance // std430 layout
{
    uint materialIndex;
    uint geometryIndex;
};

struct RenderParameters // std430 layout
{
    uint settings[4];
    uint frame[4];
    vec4 cameraPositionAndAspect;
    vec4 cameraUpVectorAndTanHalfFOV;
    vec4 cameraRightVector;
    vec4 cameraForwardVector;
    vec4 skyRadiance;
};

struct DisplayParameters // std430 layout
{
    float invGamma;
    float exposure;
    float padding[2];
};

#endif // QUARTZ_SHADERS_TYPES_H
