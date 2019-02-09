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

class GraphicsPipelineBuilder final : public PipelineBuilderImpl<GraphicsPipelineBuilder>
{
public:
    GraphicsPipelineBuilder(Device *device, VkRenderPass renderPass, uint32_t subpass=0);

    // TODO: Add pipeline configuration functions.

    Pipeline build() const;
    bool validate() const;

private:
    struct VertexInputState {
        QVector<VkVertexInputBindingDescription> bindingDescriptions;
        QVector<VkVertexInputAttributeDescription> attributeDescriptions;
    };
    struct ViewportState {
        QVector<VkViewport> viewport;
        QVector<VkRect2D> scissor;
    };
    struct ColorBlendState {
        bool logicOpEnable;
        VkLogicOp logicOp;
        QVector<VkPipelineColorBlendAttachmentState> attachments;
        float blendConstants[4];
    };

    VertexInputState m_vertexInputState;
    VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyState;
    VkPipelineTessellationStateCreateInfo m_tessellationState;
    ViewportState m_viewportState;
    VkPipelineRasterizationStateCreateInfo m_rasterizationState;
    VkPipelineMultisampleStateCreateInfo m_multisampleState;
    VkPipelineDepthStencilStateCreateInfo m_depthStencilState;
    ColorBlendState m_colorBlendState = {};
    QVector<VkDynamicState> m_dynamicStates;
    VkRenderPass m_renderPass;
    uint32_t m_subpass;

    Q_DISABLE_COPY(GraphicsPipelineBuilder)
};

} // Vulkan
} // Qt3DRaytrace
