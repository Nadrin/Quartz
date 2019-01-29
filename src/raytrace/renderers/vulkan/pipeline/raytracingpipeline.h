/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/pipeline/pipeline.h>

namespace Qt3DRaytrace {
namespace Vulkan {

struct RayTracingPipeline : Pipeline
{
    Buffer shaderBindingTable;
    VkDeviceSize shaderBindingTableStride = 0;
    VkDeviceSize shaderBindingTableHitGroupOffset = 0;
};

class RayTracingPipelineBuilder final : public PipelineBuilderImpl<RayTracingPipelineBuilder>
{
public:
    explicit RayTracingPipelineBuilder(Device *device);

    RayTracingPipelineBuilder &maxRecursionDepth(uint32_t maxDepth);

    RayTracingPipeline build() const;
    bool validate() const;

private:
    bool buildShaderGroups(QVector<VkRayTracingShaderGroupCreateInfoNV> &shaderGroups, uint32_t &firstHitGroupIndex) const;

    uint32_t m_maxRecursionDepth;

    Q_DISABLE_COPY(RayTracingPipelineBuilder)
};

} // Vulkan
} // Qt3DRaytrace
