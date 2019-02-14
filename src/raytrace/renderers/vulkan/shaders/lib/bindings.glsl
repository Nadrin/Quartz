/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_BINDINGS_H
#define QUARTZ_SHADERS_BINDINGS_H

const uint DS_Display = 0;

const uint Binding_DisplayBuffer = 0;

const uint DS_Render = 0;
const uint DS_AttributeBuffer = 1;
const uint DS_IndexBuffer = 2;

const uint Binding_TLAS = 0;
const uint Binding_Instances = 1;
const uint Binding_Materials = 2;
const uint Binding_Emitters = 3;
const uint Binding_RenderBuffer = 4;
const uint Binding_PrevRenderBuffer = 5;

const uint Shader_PathTraceHit    = 0;
const uint Shader_PathTraceMiss   = 0;
const uint Shader_SampleLightHit  = 1;
const uint Shader_SampleLightMiss = 1;

#endif // QUARTZ_SHADERS_BINDINGS_H
