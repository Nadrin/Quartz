/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_BINDINGS_H
#define QUARTZ_SHADERS_BINDINGS_H

#include "types.glsl.h"

#ifdef __cplusplus
namespace Qt3DRaytrace {
namespace Vulkan {
#endif

const uint DS_Display = 0;

const uint Binding_DisplayBuffer = 0;

const uint DS_Render = 0;
const uint DS_AttributeBuffer = 1;
const uint DS_IndexBuffer = 2;

const uint Binding_TLAS = 0;
const uint Binding_Instances = 1;
const uint Binding_Materials = 2;
const uint Binding_RenderBuffer = 3;
const uint Binding_PrevRenderBuffer = 4;

const uint FrameParam_FrameNumber = 0;
const uint FrameParam_RandomSeed = 1;

const uint RenderSetting_PrimarySamples = 0;
const uint RenderSetting_SecondarySamples = 1;
const uint RenderSetting_MaxDepth = 2;

#ifdef __cplusplus
} // Vulkan
} // Qt3DRaytrace
#endif

#endif // QUARTZ_SHADERS_BINDINGS_H
