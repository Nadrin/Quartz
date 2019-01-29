/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/pipeline/raytracingpipeline.h>
#include <renderers/vulkan/device.h>

namespace Qt3DRaytrace {
namespace Vulkan {

static VkRayTracingShaderGroupCreateInfoNV createGeneralShaderGroup(uint32_t shaderIndex)
{
    VkRayTracingShaderGroupCreateInfoNV shaderGroup = { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV };
    shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV;
    shaderGroup.generalShader = shaderIndex;
    shaderGroup.closestHitShader = VK_SHADER_UNUSED_NV;
    shaderGroup.anyHitShader = VK_SHADER_UNUSED_NV;
    shaderGroup.intersectionShader = VK_SHADER_UNUSED_NV;
    return shaderGroup;
}

static VkRayTracingShaderGroupCreateInfoNV createTrianglesHitShaderGroup(uint32_t closestHitShaderIndex, uint32_t anyHitShaderIndex)
{
    VkRayTracingShaderGroupCreateInfoNV shaderGroup = { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV };
    shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_NV;
    shaderGroup.generalShader = VK_SHADER_UNUSED_NV;
    shaderGroup.closestHitShader = closestHitShaderIndex;
    shaderGroup.anyHitShader = anyHitShaderIndex;
    shaderGroup.intersectionShader = VK_SHADER_UNUSED_NV;
    return shaderGroup;
}

RayTracingPipelineBuilder::RayTracingPipelineBuilder(Device *device)
    : PipelineBuilderImpl<RayTracingPipelineBuilder>(device)
{
    m_maxRecursionDepth = 1;
}

RayTracingPipelineBuilder &RayTracingPipelineBuilder::maxRecursionDepth(uint32_t maxDepth)
{
    if(maxDepth > m_device->rayTracingProperties().maxRecursionDepth) {
        maxDepth = m_device->rayTracingProperties().maxRecursionDepth;
        qCWarning(logVulkan) << "RayTracingPipelineBuilder: clamping maxRecursionDepth to" << maxDepth;
    }
    m_maxRecursionDepth = maxDepth;
    return *this;
}

Pipeline RayTracingPipelineBuilder::build() const
{
    Pipeline pipeline;
    pipeline.bindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_NV;

    if(!validate()) {
        return pipeline;
    }

    QVector<VkRayTracingShaderGroupCreateInfoNV> shaderGroups;
    uint32_t firstHitGroupIndex;
    if(!buildShaderGroups(shaderGroups, firstHitGroupIndex)) {
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

    VkRayTracingPipelineCreateInfoNV createInfo = { VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_NV };
    createInfo.stageCount = uint32_t(shaderStages.size());
    createInfo.pStages = shaderStages.data();
    createInfo.groupCount = uint32_t(shaderGroups.size());
    createInfo.pGroups = shaderGroups.data();
    createInfo.maxRecursionDepth = m_maxRecursionDepth;
    createInfo.layout = pipeline.pipelineLayout;

    Result result;
    if(VKFAILED(result = vkCreateRayTracingPipelinesNV(*m_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline.handle))) {
        qCCritical(logVulkan) << "RayTracingPipelineBuilder: Failed to create ray tracing pipeline:" << result.toString();
        m_device->destroyPipeline(pipeline);
        return pipeline;
    }

    const VkDeviceSize shaderGroupHandleSize = m_device->rayTracingProperties().shaderGroupHandleSize;
    const VkDeviceSize shaderBindingTableSize = shaderGroupHandleSize * VkDeviceSize(shaderGroups.size());

    BufferCreateInfo shaderBindingTableCreateInfo;
    shaderBindingTableCreateInfo.size = shaderBindingTableSize;
    shaderBindingTableCreateInfo.usage = VK_BUFFER_USAGE_RAY_TRACING_BIT_NV;
    pipeline.shaderBindingTable = m_device->createBuffer(shaderBindingTableCreateInfo, VMA_MEMORY_USAGE_CPU_ONLY);
    if(!pipeline.shaderBindingTable || !pipeline.shaderBindingTable.isHostAccessible()) {
        qCCritical(logVulkan) << "RayTracingPipelineBuilder: Failed to create pipeline shader binding table buffer";
        m_device->destroyPipeline(pipeline);
        return pipeline;
    }
    if(VKFAILED(result = vkGetRayTracingShaderGroupHandlesNV(*m_device, pipeline, 0, uint32_t(shaderGroups.size()),
                                                             shaderBindingTableSize, pipeline.shaderBindingTable.memory()))) {
        qCCritical(logVulkan) << "RayTracingPipelineBuilder: Failed to retrieve shader group handles:" << result.toString();
        m_device->destroyPipeline(pipeline);
    }

    pipeline.shaderBindingTableStride = shaderGroupHandleSize;
    pipeline.shaderBindingTableHitGroupOffset = shaderGroupHandleSize * firstHitGroupIndex;
    return pipeline;
}

bool RayTracingPipelineBuilder::validate() const
{
    if(m_shaders.empty()) {
        qCCritical(logVulkan) << "RayTracePipelineBuilder: Pipeline validation failed: no shader modules present";
        return false;
    }

    bool hasRaygenStage = false;
    for(int index=0; index < m_shaders.size(); ++index) {
        switch(m_shaders[index]->stage()) {
        case VK_SHADER_STAGE_RAYGEN_BIT_NV:
            if(hasRaygenStage) {
                qCCritical(logVulkan) << "RayTracePipelineBuilder: Pipeline validation failed: more than one VK_SHADER_STAGE_RAYGEN_BIT_NV shader module present";
                return false;
            }
            hasRaygenStage = true;
            break;
        case VK_SHADER_STAGE_MISS_BIT_NV:
        case VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV:
        case VK_SHADER_STAGE_ANY_HIT_BIT_NV:
            break;
        default:
            qCCritical(logVulkan) << "RayTracePipelineBuilder: Pipeline validation failed: incompatible shader stage of shader module" << index;
            return false;
        }
    }
    if(!hasRaygenStage) {
        qCCritical(logVulkan) << "RayTracePipelineBuilder: Pipeline validation failed: no VK_SHADER_STAGE_RAYGEN_BIT_NV present";
        return false;
    }

    // TODO: Validate shader group indices.
    return true;
}

bool RayTracingPipelineBuilder::buildShaderGroups(QVector<VkRayTracingShaderGroupCreateInfoNV> &shaderGroups, uint32_t &firstHitGroupIndex) const
{
    VkRayTracingShaderGroupCreateInfoNV raygenGroup;
    QVector<VkRayTracingShaderGroupCreateInfoNV> missGroups;
    QVector<VkRayTracingShaderGroupCreateInfoNV> hitGroups;

    bool hasRaygenShader = false;
    uint32_t pendingClosestHitShaderIndex = VK_SHADER_UNUSED_NV;
    for(uint32_t shaderIndex = 0; shaderIndex < uint32_t(m_shaders.size()); ++shaderIndex) {
        switch(m_shaders[int(shaderIndex)]->stage()) {
        case VK_SHADER_STAGE_RAYGEN_BIT_NV:
            Q_ASSERT(!hasRaygenShader);
            raygenGroup = createGeneralShaderGroup(shaderIndex);
            hasRaygenShader = true;
            break;
        case VK_SHADER_STAGE_MISS_BIT_NV:
            if(pendingClosestHitShaderIndex != VK_SHADER_UNUSED_NV) {
                hitGroups.append(createTrianglesHitShaderGroup(pendingClosestHitShaderIndex, VK_SHADER_UNUSED_NV));
                pendingClosestHitShaderIndex = VK_SHADER_UNUSED_NV;
            }
            missGroups.append(createGeneralShaderGroup(shaderIndex));
            break;
        case VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV:
            if(pendingClosestHitShaderIndex != VK_SHADER_UNUSED_NV) {
                hitGroups.append(createTrianglesHitShaderGroup(pendingClosestHitShaderIndex, VK_SHADER_UNUSED_NV));
            }
            pendingClosestHitShaderIndex = shaderIndex;
            break;
        case VK_SHADER_STAGE_ANY_HIT_BIT_NV:
            if(pendingClosestHitShaderIndex != VK_SHADER_UNUSED_NV) {
                hitGroups.append(createTrianglesHitShaderGroup(pendingClosestHitShaderIndex, shaderIndex));
                pendingClosestHitShaderIndex = VK_SHADER_UNUSED_NV;
            }
            else {
                qCWarning(logVulkan) << "RayTracePipelineBuilder: Unmatched any-hit shader module won't be used in shader binding table:" << shaderIndex;
            }
            break;
        default:
            Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid shader stage");
            break;
        }
    }

    Q_ASSERT(hasRaygenShader);

    shaderGroups.reserve(1 + missGroups.size() + hitGroups.size());
    shaderGroups.append(raygenGroup);
    shaderGroups.append(missGroups);
    shaderGroups.append(hitGroups);

    firstHitGroupIndex = 1 + uint32_t(missGroups.size());
    return true;
}

} // Vulkan
} // Qt3DRaytrace
