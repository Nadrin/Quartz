/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/managers/stagingresourcemanager.h>
#include <renderers/vulkan/device.h>

#include <QMutexLocker>
#include <QMutableVectorIterator>

namespace Qt3DRaytrace {
namespace Vulkan {

StagingResourceManager::StagingResourceManager(Device *device)
    : m_device(device)
{
    Q_ASSERT(m_device);
}

StagingResourceManager::~StagingResourceManager()
{
    cleanup();
    if(m_buffers.size() > 0) {
        qCWarning(logVulkan) << "StagingResourceManager:" << m_buffers.size() << "orphaned staging buffers";
    }
}

StagingBuffer StagingResourceManager::acquireStagingBuffer(VkDeviceSize size)
{
    StagingBuffer buffer;
    buffer.resource = m_device->createBuffer({size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT}, VMA_MEMORY_USAGE_CPU_ONLY);
    if(!buffer) {
        qCCritical(logVulkan) << "StagingResourceManager: Cannot allocate staging buffer";
        return StagingBuffer();
    }
    buffer.releaseEvent = m_device->createEvent();
    if(!buffer.releaseEvent) {
        m_device->destroyBuffer(buffer.resource);
        return StagingBuffer();
    }
    buffer.mappedAddress = m_device->mapMemory(buffer.resource.allocation);
    if(!buffer.mappedAddress) {
        m_device->destroyBuffer(buffer.resource);
        m_device->destroyEvent(buffer.releaseEvent);
        return StagingBuffer();
    }

    return buffer;
}

void StagingResourceManager::releaseStagingBuffer(CommandBuffer commandBuffer, StagingBuffer &buffer)
{
    m_device->unmapMemory(buffer.resource.allocation);
    commandBuffer.setEvent(buffer.releaseEvent, VK_PIPELINE_STAGE_TRANSFER_BIT);

    QMutexLocker lock(&m_mutex);
    m_buffers.append(buffer);
    buffer = {};
}

void StagingResourceManager::proceedToNextFrame()
{
    cleanup();
}

void StagingResourceManager::cleanup()
{
    QMutexLocker lock(&m_mutex);

    QVector<StagingBuffer> buffersToDestroy;
    buffersToDestroy.reserve(m_buffers.size());

    QMutableVectorIterator<StagingBuffer> it(m_buffers);
    while(it.hasNext()) {
        auto buffer = it.next();
        if(m_device->isEventSignaled(buffer.releaseEvent)) {
            buffersToDestroy.append(buffer);
            it.remove();
        }
    }
    lock.unlock();

    for(StagingBuffer &buffer : buffersToDestroy) {
        m_device->destroyBuffer(buffer.resource);
        m_device->destroyEvent(buffer.releaseEvent);
    }
}

} // Vulkan
} // Qt3DRaytrace
