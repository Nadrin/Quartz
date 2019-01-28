/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/jobs/buildscenetlasjob.h>
#include <renderers/vulkan/renderer.h>

#include <backend/managers_p.h>
#include <backend/entity_p.h>
#include <backend/geometryrenderer_p.h>

#include <cstring>
#include <QVector>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Vulkan {

BuildSceneTopLevelAccelerationStructureJob::BuildSceneTopLevelAccelerationStructureJob(Renderer *renderer, Raytrace::NodeManagers *managers)
    : m_renderer(renderer)
    , m_nodeManagers(managers)
{
    Q_ASSERT(m_renderer);
    Q_ASSERT(m_nodeManagers);
}

void BuildSceneTopLevelAccelerationStructureJob::run()
{
    auto *device = m_renderer->device();
    auto *commandBufferManager = m_renderer->commandBufferManager();

    Buffer instanceBuffer;
    uint32_t numInstances;
    {
        const QVector<GeometryInstance> instances = gatherGeometryInstances();
        numInstances = uint32_t(instances.size());
        if(numInstances == 0) {
            return;
        }

        BufferCreateInfo instanceBufferCreateInfo;
        instanceBufferCreateInfo.size = sizeof(GeometryInstance) * numInstances;
        instanceBufferCreateInfo.usage = VK_BUFFER_USAGE_RAY_TRACING_BIT_NV;
        instanceBuffer = device->createBuffer(instanceBufferCreateInfo, VMA_MEMORY_USAGE_CPU_ONLY);
        if(!instanceBuffer || !instanceBuffer.isHostAccessible()) {
            qCCritical(logVulkan) << "Failed to create geometry instance buffer in host accessible memory";
            return;
        }
        std::memcpy(instanceBuffer.memory(), instances.data(), instanceBufferCreateInfo.size);
    }

    VkAccelerationStructureInfoNV tlasInfo = { VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV };
    tlasInfo.type  = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
    tlasInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV;
    tlasInfo.instanceCount = numInstances;

    AccelerationStructure tlas = device->createAccelerationStructure(tlasInfo);
    if(!tlas) {
        qCCritical(logVulkan) << "Failed to create scene TLAS";
        device->destroyBuffer(instanceBuffer);
        return;
    }

    Device::ScratchBufferType scratchBufferType = Device::ScratchBufferType::Build;
    AccelerationStructure previousTLAS = m_renderer->sceneTLAS();
    if(previousTLAS) {
        scratchBufferType = Device::ScratchBufferType::Update;
    }

    Buffer scratchBuffer = device->createAccelerationStructureScratchBuffer(tlas, scratchBufferType);
    if(!scratchBuffer) {
        qCCritical(logVulkan) << "Failed to create TLAS build scratch buffer";
        device->destroyBuffer(instanceBuffer);
        device->destroyAccelerationStructure(tlas);
        return;
    }

    TransientCommandBuffer commandBuffer = commandBufferManager->acquireCommandBuffer();
    {
        commandBuffer->buildTopLevelAccelerationStructure(tlasInfo, tlas, previousTLAS, instanceBuffer, scratchBuffer);
        commandBuffer->pipelineBarrier(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV,
                                       VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV, VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV);
    }
    commandBufferManager->releaseCommandBuffer(commandBuffer, {instanceBuffer, scratchBuffer});

    m_renderer->updateSceneTLAS(tlas);
}

QVector<GeometryInstance> BuildSceneTopLevelAccelerationStructureJob::gatherGeometryInstances() const
{
    QVector<GeometryInstance> instances;

    // TODO: Prefilter elligible entities in a dedicated job.
    for(const auto &entity : m_nodeManagers->entityManager.activeHandles()) {
        const Raytrace::GeometryRenderer *geometryRenderer = entity->geometryRendererComponent();
        if(geometryRenderer && !geometryRenderer->geometryId().isNull()) {
            uint64_t blasHandle = m_renderer->lookupBLAS(geometryRenderer->geometryId());
            if(blasHandle != 0) {
                const QMatrix4x4 worldTransformRowMajor = entity->worldTransformMatrix.transposed().toQMatrix4x4();
                GeometryInstance geometryInstance = {};
                std::memcpy(geometryInstance.transform, worldTransformRowMajor.constData(), sizeof(geometryInstance.transform));
                geometryInstance.mask = 0xFF;
                geometryInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_CULL_DISABLE_BIT_NV; // Temp!
                geometryInstance.blasHandle = blasHandle;
                instances.append(geometryInstance);
            }
        }
    }
    return instances;
}

} // Vulkan
} // Qt3DRaytrace
