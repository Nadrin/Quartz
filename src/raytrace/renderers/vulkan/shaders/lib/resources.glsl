/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_RESOURCES_H
#define QUARTZ_SHADERS_RESOURCES_H

#include "common.glsl"
#include "geometry.glsl"

layout(set=DS_Render, binding=Binding_Instances, std430) readonly buffer InstanceBuffer {
    EntityInstance instances[];
} instanceBuffer;

layout(set=DS_Render, binding=Binding_Materials, std430) readonly buffer MaterialBuffer {
    Material materials[];
} materialBuffer;

layout(set=DS_Render, binding=Binding_Emitters, std430) readonly buffer EmitterBuffer {
    Emitter emitters[];
} emitterBuffer;

layout(set=DS_AttributeBuffer, binding=0, std430) readonly buffer AttributeBuffer {
    Attributes attributes[];
} attributeBuffer[];

layout(set=DS_IndexBuffer, binding=0, std430) readonly buffer IndexBuffer {
    Face faces[];
} indexBuffer[];

Triangle fetchTriangle(uint geometryIndex, uint faceIndex)
{
    const Face face = indexBuffer[nonuniformEXT(geometryIndex)].faces[faceIndex];

    Triangle result;
    result.v1 = attributeBuffer[nonuniformEXT(geometryIndex)].attributes[face.vertices[0]];
    result.v2 = attributeBuffer[nonuniformEXT(geometryIndex)].attributes[face.vertices[1]];
    result.v3 = attributeBuffer[nonuniformEXT(geometryIndex)].attributes[face.vertices[2]];
    return result;
}

EntityInstance fetchInstance(uint instanceIndex)
{
    return instanceBuffer.instances[instanceIndex];
}

Material fetchMaterial(uint instanceIndex)
{
    uint materialIndex = instanceBuffer.instances[instanceIndex].materialIndex;
    return materialBuffer.materials[materialIndex];
}

Emitter fetchEmitter(uint emitterIndex)
{
    return emitterBuffer.emitters[emitterIndex];
}

#endif // QUARTZ_SHADERS_RESOURCES_H
