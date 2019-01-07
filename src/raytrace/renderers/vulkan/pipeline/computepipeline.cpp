/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/pipeline/computepipeline.h>

namespace Qt3DRaytrace {
namespace Vulkan {

ComputePipelineBuilder::ComputePipelineBuilder(Device *device)
    : PipelineBuilderImpl<ComputePipelineBuilder>(device)
{}

Pipeline ComputePipelineBuilder::build() const
{
    Pipeline pipeline(VK_PIPELINE_BIND_POINT_COMPUTE);
    if(!validate()) {
        return pipeline;
    }

    QVector<VkDescriptorSetLayout> descriptorSetLayouts = buildDescriptorSetLayouts();
    VkPipelineLayout pipelineLayout = buildPipelineLayout(descriptorSetLayouts);
    if(pipelineLayout == VK_NULL_HANDLE) {
        for(VkDescriptorSetLayout layout : descriptorSetLayouts) {
            vkDestroyDescriptorSetLayout(m_device, layout, nullptr);
        }
        return pipeline;
    }

    // TODO: Add support for specialization constants.
    VkPipelineShaderStageCreateInfo shaderStage = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    shaderStage.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStage.module = m_shaders[0]->module();
    shaderStage.pName  = m_shaders[0]->entryPoint().data();

    VkComputePipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
    createInfo.stage = shaderStage;
    createInfo.layout = pipelineLayout;
    if(VKFAILED(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline.handle))) {
        qCCritical(logVulkan) << "ComputePipelineBuilder: Failed to create compute pipeline";
        vkDestroyPipelineLayout(m_device, pipelineLayout, nullptr);
        for(VkDescriptorSetLayout layout : descriptorSetLayouts) {
            vkDestroyDescriptorSetLayout(m_device, layout, nullptr);
        }
        return pipeline;
    }

    pipeline.pipelineLayout = pipelineLayout;
    pipeline.descriptorSetLayouts = std::move(descriptorSetLayouts);
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
