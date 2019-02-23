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

UpdateMaterialsJob::UpdateMaterialsJob(Renderer *renderer, Raytrace::TextureManager *textureManager)
    : m_renderer(renderer)
    , m_textureManager(textureManager)
{
    Q_ASSERT(m_renderer);
    Q_ASSERT(m_textureManager);
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

    auto lookupTextureImageIndex = [this, sceneManager](QNodeId textureId) -> uint32_t {
        if(const auto *texture = m_textureManager->lookupResource(textureId)) {
            return sceneManager->lookupTextureIndex(texture->imageId());
        }
        return ~0u;
    };

    for(const auto &handle : m_dirtyMaterialHandles) {
        Raytrace::Material *material = handle.data();

        Material materialData;
        material->albedo().writeToBuffer(materialData.albedo.data);
        material->emission().writeToBuffer(materialData.emission.data);
        // Pack roughness in albedo.a
        materialData.albedo.data[3] = material->roughness();
        // Pack metalness in emission.a
        materialData.emission.data[3] = material->metalness();

        materialData.albedoTexture = lookupTextureImageIndex(material->albedoTextureId());
        materialData.roughnessTexture = lookupTextureImageIndex(material->roughnessTextureId());
        materialData.metalnessTexture = lookupTextureImageIndex(material->metalnessTextureId());

        // TODO: Reduce lock contention on rwlock.
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
    commandBufferManager->releaseCommandBuffer(commandBuffer, QVector<Buffer>{stagingBuffer});

    sceneManager->updateMaterialBuffer(materialBuffer);
}

} // Vulkan
} // Qt3DRaytrace
