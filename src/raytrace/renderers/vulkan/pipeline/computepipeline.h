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

class ComputePipelineBuilder final : public PipelineBuilderImpl<ComputePipelineBuilder>
{
public:
    explicit ComputePipelineBuilder(Device *device);

    Pipeline build() const;
    bool validate() const;

private:
    Q_DISABLE_COPY(ComputePipelineBuilder)
};

} // Vulkan
} // Qt3DRaytrace
