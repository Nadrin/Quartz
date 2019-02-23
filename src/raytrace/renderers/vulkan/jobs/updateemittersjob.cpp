/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/jobs/updateemittersjob.h>
#include <renderers/vulkan/renderer.h>

#include <backend/managers_p.h>
#include <backend/rendersettings_p.h>

#include <QVector>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Vulkan {

UpdateEmittersJob::UpdateEmittersJob(Renderer *renderer)
    : m_renderer(renderer)
{
    Q_ASSERT(m_renderer);
}

void UpdateEmittersJob::run()
{
    auto *device = m_renderer->device();
    auto *commandBufferManager = m_renderer->commandBufferManager();
    auto *sceneManager = m_renderer->sceneManager();

    // TODO: Update only dirty emissive entities.
    // Currently there's no meaningful semantic determining which entities
    // are dirty in the context of becoming emitters.

    QVector<Emitter> emitters;
    {
        Emitter skyEmitter = {};
        skyEmitter.instanceIndex = ~0u;
        if(m_renderer->settings()) {
            m_renderer->settings()->skyRadiance().writeToBuffer(skyEmitter.radiance.data);
        }
        emitters.append(skyEmitter);
    }

    for(const auto &entity : sceneManager->emissives()) {
        const QMatrix4x4 entityTransform = entity->worldTransformMatrix.toQMatrix4x4();
        if(!entity->distantLightComponentId().isNull()) {
            const Raytrace::DistantLight *light = entity->distantLightComponent();
            Q_ASSERT(light);

            const QVector3D worldDirection = entityTransform.mapVector(light->direction()).normalized();

            Emitter emitter = {};
            emitter.instanceIndex = ~0u;
            emitter.direction = worldDirection;
            light->radiance().writeToBuffer(emitter.radiance.data);
            emitters.append(emitter);
        }
        if(entity->isRenderable()) {
            const Raytrace::Material *material = entity->materialComponent();
            const Raytrace::GeometryRenderer *geometryRenderer = entity->geometryRendererComponent();
            Q_ASSERT(material && geometryRenderer);

            Emitter emitter = {};
            emitter.instanceIndex = sceneManager->lookupRenderableIndex(entity->peerId());
            emitter.geometryIndex = sceneManager->lookupGeometryIndex(geometryRenderer->geometryId());
            material->emission().writeToBuffer(emitter.radiance.data);
            emitters.append(emitter);
        }
    }
    Q_ASSERT(emitters.size() >= 1);

    const VkDeviceSize emitterBufferSize = sizeof(Emitter) * uint32_t(emitters.size());

    BufferCreateInfo emitterBufferCreateInfo;
    emitterBufferCreateInfo.size = emitterBufferSize;
    emitterBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    Buffer emitterBuffer = device->createBuffer(emitterBufferCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY);
    if(!emitterBuffer) {
        qCCritical(logVulkan) << "Failed to create emitter data buffer";
        return;
    }

    Buffer stagingBuffer = device->createStagingBuffer(emitterBufferSize);
    if(!stagingBuffer || !stagingBuffer.isHostAccessible()) {
        qCCritical(logVulkan) << "Failed to create staging buffer for material data upload";
        device->destroyBuffer(emitterBuffer);
        return;
    }

    std::memcpy(stagingBuffer.memory(), emitters.data(), emitterBufferSize);

    TransientCommandBuffer commandBuffer = commandBufferManager->acquireCommandBuffer();
    {
        commandBuffer->copyBuffer(stagingBuffer, 0, emitterBuffer, 0, emitterBufferSize);
        commandBuffer->resourceBarrier({emitterBuffer, BufferState::CopyDest, BufferState::ShaderRead});
    }
    commandBufferManager->releaseCommandBuffer(commandBuffer, QVector<Buffer>{stagingBuffer});

    sceneManager->updateEmitters(emitters);
    sceneManager->updateEmitterBuffer(emitterBuffer);
}

} // Vulkan
} // Qt3DRaytrace
