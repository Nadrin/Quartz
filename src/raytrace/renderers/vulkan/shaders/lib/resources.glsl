/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_RESOURCES_H
#define QUARTZ_SHADERS_RESOURCES_H

#include "bindings.glsl"
#include "shared.glsl"
#include "rayhit.glsl"

layout(set=DS_Render, binding=Binding_Instances, std430) readonly buffer InstanceBuffer {
    EntityInstance instances[];
} instanceBuffer;

layout(set=DS_Render, binding=Binding_Materials, std430) readonly buffer MaterialBuffer {
    Material materials[];
} materialBuffer;

layout(set=DS_AttributeBuffer, binding=0, std430) readonly buffer AttributeBuffer {
    Attributes attributes[];
} attributeBuffer[];

layout(set=DS_IndexBuffer, binding=0, std430) readonly buffer IndexBuffer {
    Face faces[];
} indexBuffer[];

Triangle fetchTriangle()
{
    const Face face = indexBuffer[nonuniformEXT(gl_InstanceCustomIndexNV)].faces[gl_PrimitiveID];

    Triangle result;
    result.v1 = attributeBuffer[nonuniformEXT(gl_InstanceCustomIndexNV)].attributes[face.vertices[0]];
    result.v2 = attributeBuffer[nonuniformEXT(gl_InstanceCustomIndexNV)].attributes[face.vertices[1]];
    result.v3 = attributeBuffer[nonuniformEXT(gl_InstanceCustomIndexNV)].attributes[face.vertices[2]];
    return result;
}

Material fetchMaterial()
{
    const EntityInstance instance = instanceBuffer.instances[gl_InstanceID];
    return materialBuffer.materials[instance.materialIndex];
}

#endif // QUARTZ_SHADERS_RESOURCES_H
