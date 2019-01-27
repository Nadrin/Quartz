/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/jobs/buildgeometryjob.h>
#include <renderers/vulkan/renderer.h>
#include <renderers/vulkan/geometry.h>

#include <backend/managers_p.h>
#include <backend/geometry_p.h>

#include <cstring>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Vulkan {

BuildGeometryJob::BuildGeometryJob(Renderer *renderer, Raytrace::NodeManagers *managers, const Raytrace::HGeometry &handle)
    : m_renderer(renderer)
    , m_nodeManagers(managers)
    , m_handle(handle)
{
    Q_ASSERT(m_renderer);
    Q_ASSERT(m_nodeManagers);
}

void BuildGeometryJob::run()
{
    Raytrace::Geometry *geometryNode = m_nodeManagers->geometryManager.data(m_handle);
    if(!geometryNode) {
        return;
    }

    auto *device = m_renderer->device();
    auto *commandBufferManager = m_renderer->commandBufferManager();

    Geometry geometry;
    geometry.numVertices = uint32_t(geometryNode->vertices().size());
    geometry.numIndices = uint32_t(geometryNode->faces().size()) * 3;

    const VkDeviceSize attributeBufferSize = sizeof(QVertex) * geometry.numVertices;
    const VkDeviceSize indexBufferSize = sizeof(uint32_t) * geometry.numIndices;

    BufferCreateInfo attributeBufferCreateInfo;
    attributeBufferCreateInfo.size = attributeBufferSize;
    attributeBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_RAY_TRACING_BIT_NV;
    geometry.attributes = device->createBuffer(attributeBufferCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY);
    if(!geometry.attributes) {
        qCCritical(logVulkan) << "Failed to create geometry attribute buffer";
        return;
    }

    BufferCreateInfo indexBufferCreateInfo;
    indexBufferCreateInfo.size = indexBufferSize;
    indexBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_RAY_TRACING_BIT_NV;
    geometry.indices = device->createBuffer(indexBufferCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY);
    if(!geometry.indices) {
        qCCritical(logVulkan) << "Failed to create geometry index buffer";
        device->destroyGeometry(geometry);
        return;
    }

    VkAccelerationStructureInfoNV blasInfo = { VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV };
    VkGeometryNV blasGeometry = { VK_STRUCTURE_TYPE_GEOMETRY_NV };
    {
        VkGeometryTrianglesNV blasGeometryTriangles = { VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV };
        blasGeometryTriangles.vertexData = geometry.attributes;
        blasGeometryTriangles.vertexCount = geometry.numVertices;
        blasGeometryTriangles.vertexStride = sizeof(QVertex);
        blasGeometryTriangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        blasGeometryTriangles.indexData = geometry.indices;
        blasGeometryTriangles.indexCount = geometry.numIndices;
        blasGeometryTriangles.indexType = VK_INDEX_TYPE_UINT32; // TODO: Automatically switch to UINT16 on small meshes.

        // Note: Unused.
        VkGeometryAABBNV blasGeometryAABB = { VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV };

        blasGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_NV;
        blasGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_NV; // TODO: Investigate how to best handle non-opaque meshes.
        blasGeometry.geometry.triangles = blasGeometryTriangles;
        blasGeometry.geometry.aabbs = blasGeometryAABB;

        blasInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
        blasInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;
        blasInfo.geometryCount = 1;
        blasInfo.pGeometries = &blasGeometry;

        geometry.blas = device->createAccelerationStructure(blasInfo);
        if(!geometry.blas) {
            qCCritical(logVulkan) << "Failed to create geometry BLAS";
            device->destroyGeometry(geometry);
            return;
        }

        vkGetAccelerationStructureHandleNV(*device, geometry.blas, sizeof(geometry.blasHandle), &geometry.blasHandle);
    }

    Buffer scratchBuffer = device->createAccelerationStructureScratchBuffer(geometry.blas, Device::ScratchBufferType::Build);
    if(!scratchBuffer) {
        qCCritical(logVulkan) << "Failed to create BLAS build scratch buffer";
        device->destroyGeometry(geometry);
        return;
    }

    Buffer stagingAttributes = device->createStagingBuffer(attributeBufferSize);
    Buffer stagingIndices = device->createStagingBuffer(indexBufferSize);
    if(!stagingAttributes || !stagingIndices) {
        qCCritical(logVulkan) << "Failed to create staging buffers for BLAS build";
        device->destroyBuffer(stagingAttributes);
        device->destroyBuffer(stagingIndices);
        device->destroyBuffer(scratchBuffer);
        device->destroyGeometry(geometry);
        return;
    }

    std::memcpy(stagingAttributes.memory(), geometryNode->vertices().data(), attributeBufferSize);
    std::memcpy(stagingIndices.memory(), geometryNode->faces().data(), indexBufferSize);

    TransientCommandBuffer commandBuffer = commandBufferManager->acquireCommandBuffer();
    {
        commandBuffer->copyBuffer(stagingAttributes, 0, geometry.attributes, 0, attributeBufferSize);
        commandBuffer->copyBuffer(stagingIndices, 0, geometry.indices, 0, indexBufferSize);
        commandBuffer->pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                       VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_ACCESS_MEMORY_READ_BIT);
        commandBuffer->buildAccelerationStructure(blasInfo, geometry.blas, scratchBuffer);
        commandBuffer->pipelineBarrier(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV,
                                       VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV, VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV);
    }
    commandBufferManager->releaseCommandBuffer(commandBuffer, {stagingAttributes, stagingIndices, scratchBuffer});

    m_renderer->addSceneGeometry(geometry);
}

} // Vulkan
} // Qt3DRaytrace
