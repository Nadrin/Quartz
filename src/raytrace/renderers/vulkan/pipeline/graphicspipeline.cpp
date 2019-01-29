/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/pipeline/graphicspipeline.h>
#include <renderers/vulkan/device.h>

namespace Qt3DRaytrace {
namespace Vulkan {

static constexpr VkPipelineColorBlendAttachmentState DefaultBlendAttachmentState = {
    VK_FALSE,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_A_BIT,
};

GraphicsPipelineBuilder::GraphicsPipelineBuilder(Device *device, VkRenderPass renderPass, uint32_t subpass)
    : PipelineBuilderImpl<GraphicsPipelineBuilder>(device)
    , m_renderPass(renderPass)
    , m_subpass(subpass)
{
    m_inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    m_inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    m_tessellationState = { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };

    m_rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    m_rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    m_rasterizationState.cullMode = VK_CULL_MODE_NONE;
    m_rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    m_rasterizationState.lineWidth = 1.0f;

    m_multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    m_multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    m_depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
    m_depthStencilState.depthTestEnable = VK_FALSE;
    m_depthStencilState.depthWriteEnable = VK_TRUE;
    m_depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    // TODO: Configure default stencil state.

    m_colorBlendState.attachments.resize(1);
    m_colorBlendState.attachments[0] = DefaultBlendAttachmentState;

    m_dynamicStates.resize(2);
    m_dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    m_dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
}

Pipeline GraphicsPipelineBuilder::build() const
{
    Pipeline pipeline;
    pipeline.bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    if(!validate()) {
        return pipeline;
    }

    pipeline.descriptorSetLayouts = buildDescriptorSetLayouts();
    pipeline.pipelineLayout = buildPipelineLayout(pipeline.descriptorSetLayouts);
    if(pipeline.pipelineLayout == VK_NULL_HANDLE) {
        m_device->destroyPipeline(pipeline);
        return pipeline;
    }

    QVector<VkPipelineShaderStageCreateInfo> shaderStages(m_shaders.size());
    for(int i=0; i<m_shaders.size(); ++i) {
        shaderStages[i].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[i].stage  = m_shaders[i]->stage();
        shaderStages[i].module = m_shaders[i]->module();
        shaderStages[i].pName  = m_shaders[i]->entryPoint().data();
    }

    VkPipelineVertexInputStateCreateInfo vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    vertexInputState.vertexBindingDescriptionCount = uint32_t(m_vertexInputState.bindingDescriptions.size());
    if(vertexInputState.vertexBindingDescriptionCount > 0) {
        vertexInputState.pVertexBindingDescriptions = m_vertexInputState.bindingDescriptions.data();
    }
    vertexInputState.vertexAttributeDescriptionCount = uint32_t(m_vertexInputState.attributeDescriptions.size());
    if(vertexInputState.vertexAttributeDescriptionCount > 0) {
        vertexInputState.pVertexAttributeDescriptions = m_vertexInputState.attributeDescriptions.data();
    }

    VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewportState.viewportCount = uint32_t(m_viewportState.viewport.size());
    viewportState.scissorCount = uint32_t(m_viewportState.scissor.size());
    Q_ASSERT(viewportState.viewportCount == viewportState.scissorCount);
    if(viewportState.viewportCount > 0) {
        viewportState.pViewports = m_viewportState.viewport.data();
        viewportState.pScissors = m_viewportState.scissor.data();
    }
    else if(m_dynamicStates.contains(VK_DYNAMIC_STATE_VIEWPORT) &&
            m_dynamicStates.contains(VK_DYNAMIC_STATE_SCISSOR)) {
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
    }

    VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    colorBlendState.logicOpEnable = m_colorBlendState.logicOpEnable ? VK_TRUE : VK_FALSE;
    colorBlendState.logicOp = m_colorBlendState.logicOp;
    colorBlendState.attachmentCount = uint32_t(m_colorBlendState.attachments.size());
    colorBlendState.pAttachments = m_colorBlendState.attachments.data();
    for(int i=0; i<4; ++i) {
        colorBlendState.blendConstants[i] = m_colorBlendState.blendConstants[i];
    }

    VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamicState.dynamicStateCount = uint32_t(m_dynamicStates.size());
    dynamicState.pDynamicStates = m_dynamicStates.data();

    VkGraphicsPipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    createInfo.stageCount = uint32_t(shaderStages.size());
    createInfo.pStages = shaderStages.data();
    createInfo.pVertexInputState = &vertexInputState;
    createInfo.pInputAssemblyState = &m_inputAssemblyState;
    createInfo.pTessellationState = &m_tessellationState;
    createInfo.pViewportState = &viewportState;
    createInfo.pRasterizationState = &m_rasterizationState;
    createInfo.pMultisampleState = &m_multisampleState;
    createInfo.pDepthStencilState = &m_depthStencilState;
    createInfo.pColorBlendState = &colorBlendState;
    createInfo.pDynamicState = &dynamicState;
    createInfo.layout = pipeline.pipelineLayout;
    createInfo.renderPass = m_renderPass;
    createInfo.subpass = m_subpass;

    Result result;
    if(VKFAILED(result = vkCreateGraphicsPipelines(*m_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline.handle))) {
        qCCritical(logVulkan) << "GraphicsPipelineBuilder: Failed to create graphics pipeline" << result.toString();
        m_device->destroyPipeline(pipeline);
    }
    return pipeline;
}

bool GraphicsPipelineBuilder::validate() const
{
    if(m_shaders.empty()) {
        qCCritical(logVulkan) << "GraphicsPipelineBuilder: Pipeline validation failed: no shader modules present";
        return false;
    }

    VkShaderStageFlags shaderStages = 0;
    for(const ShaderModule *module : m_shaders) {
        if(module->stage() == VK_SHADER_STAGE_COMPUTE_BIT) {
            qCCritical(logVulkan) << "GraphicsPipelineBuilder: Pipeline validation failed: cannot use module of shader stage VK_SHADER_STAGE_COMPUTE_BIT in a graphics pipeline";
            return false;
        }
        if((module->stage() & shaderStages) != 0) {
            qCCritical(logVulkan) << "GraphicsPipelineBuilder: Pipeline validation failed: more than one shader module for the same shader stage is present";
            return false;
        }
        shaderStages |= module->stage();
    }
    return true;
}

} // Vulkan
} // Qt3DRaytrace
