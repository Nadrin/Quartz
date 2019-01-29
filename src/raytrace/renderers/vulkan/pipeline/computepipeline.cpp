/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/pipeline/computepipeline.h>
#include <renderers/vulkan/device.h>

namespace Qt3DRaytrace {
namespace Vulkan {

ComputePipelineBuilder::ComputePipelineBuilder(Device *device)
    : PipelineBuilderImpl<ComputePipelineBuilder>(device)
{}

Pipeline ComputePipelineBuilder::build() const
{
    Pipeline pipeline;
    pipeline.bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

    if(!validate()) {
        return pipeline;
    }

    pipeline.descriptorSetLayouts = buildDescriptorSetLayouts();
    pipeline.pipelineLayout = buildPipelineLayout(pipeline.descriptorSetLayouts);
    if(pipeline.pipelineLayout == VK_NULL_HANDLE) {
        m_device->destroyPipeline(pipeline);
        return pipeline;
    }

    // TODO: Add support for specialization constants.
    VkPipelineShaderStageCreateInfo shaderStage = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    shaderStage.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStage.module = m_shaders[0]->module();
    shaderStage.pName  = m_shaders[0]->entryPoint().data();

    VkComputePipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
    createInfo.stage = shaderStage;
    createInfo.layout = pipeline.pipelineLayout;

    Result result;
    if(VKFAILED(result = vkCreateComputePipelines(*m_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline.handle))) {
        qCCritical(logVulkan) << "ComputePipelineBuilder: Failed to create compute pipeline" << result.toString();
        m_device->destroyPipeline(pipeline);
    }
    return pipeline;
}

bool ComputePipelineBuilder::validate() const
{
    if(m_shaders.empty()) {
        qCCritical(logVulkan) << "ComputePipelineBuilder: Pipeline validation failed: no shader modules present";
        return false;
    }
    if(m_shaders.size() > 1 || m_shaders[0]->stage() != VK_SHADER_STAGE_COMPUTE_BIT) {
        qCCritical(logVulkan) << "ComputePipelineBuilder: Pipeline validation failed: compute pipelines require a single shader module of VK_SHADER_STAGE_COMPUTE_BIT";
        return false;
    }
    return true;
}

} // Vulkan
} // Qt3DRaytrace
