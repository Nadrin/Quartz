/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/jobs/updatematerialsjob.h>
#include <renderers/vulkan/renderer.h>

#include <backend/managers_p.h>

#include <cstring>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Vulkan {

UpdateMaterialsJob::UpdateMaterialsJob(Renderer *renderer)
    : m_renderer(renderer)
{
    Q_ASSERT(m_renderer);
}

void UpdateMaterialsJob::setDirtyMaterialHandles(QVector<Raytrace::HMaterial> &materialHandles)
{
    m_dirtyMaterialHandles = std::move(materialHandles);
}

void UpdateMaterialsJob::run()
{
    auto *device = m_renderer->device();
    auto *commandBufferManager = m_renderer->commandBufferManager();
    auto *sceneManager = m_renderer->sceneManager();

    for(const auto &handle : m_dirtyMaterialHandles) {
        Raytrace::Material *material = handle.data();

        Material materialData;
        material->albedo().writeToBuffer(materialData.albedo.data);
        material->emission().writeToBuffer(materialData.emission.data);

        // TODO: Reduce lock contention on mutex.
        sceneManager->addOrUpdateMaterial(material->peerId(), materialData);
    }
    m_dirtyMaterialHandles.clear();

    QVector<Material> materials = sceneManager->materials();
    const VkDeviceSize materialBufferSize = sizeof(Material) * uint32_t(materials.size());
    if(materials.size() == 0) {
        return;
    }

    BufferCreateInfo materialBufferCreateInfo;
    materialBufferCreateInfo.size = materialBufferSize;
    materialBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    Buffer materialBuffer = device->createBuffer(materialBufferCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY);
    if(!materialBuffer) {
        qCCritical(logVulkan) << "Failed to create material data buffer";
        return;
    }

    Buffer stagingBuffer = device->createStagingBuffer(materialBufferSize);
    if(!stagingBuffer || !stagingBuffer.isHostAccessible()) {
        qCCritical(logVulkan) << "Failed to create staging buffer for material data upload";
        device->destroyBuffer(materialBuffer);
        return;
    }

    std::memcpy(stagingBuffer.memory(), materials.data(), materialBufferSize);

    TransientCommandBuffer commandBuffer = commandBufferManager->acquireCommandBuffer();
    {
        commandBuffer->copyBuffer(stagingBuffer, 0, materialBuffer, 0, materialBufferSize);
        commandBuffer->resourceBarrier({materialBuffer, BufferState::CopyDest, BufferState::ShaderRead});
    }
    commandBufferManager->releaseCommandBuffer(commandBuffer, {stagingBuffer});

    sceneManager->updateMaterialBuffer(materialBuffer);
}

} // Vulkan
} // Qt3DRaytrace
