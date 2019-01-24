/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/managers/commandbuffermanager.h>
#include <renderers/vulkan/device.h>
#include <renderers/vulkan/renderer.h>

#include <QMutexLocker>
#include <QMutableVectorIterator>

namespace Qt3DRaytrace {
namespace Vulkan {

CommandBufferManager::CommandBufferManager(Device *device)
    : m_device(device)
{
    Q_ASSERT(m_device);
}

CommandBufferManager::~CommandBufferManager()
{
    cleanup(false);
    if(m_pendingCommandBuffers.size() > 0) {
        qCWarning(logVulkan) << "CommandBufferManager:" << m_pendingCommandBuffers.size() << "orphaned pending batches";
    }
    for(CommandPool commandPool : m_commandPools) {
        m_device->destroyCommandPool(commandPool);
    }
}

TransientCommandBuffer CommandBufferManager::acquireCommandBuffer()
{
    if(!m_localCommandPool.hasLocalData()) {
        CommandPool commandPool = m_device->createCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
        m_localCommandPool.setLocalData(commandPool);

        QMutexLocker lock(&m_mutex);
        m_commandPools.append(commandPool);
    }

    CommandPool commandPool = m_localCommandPool.localData();
    auto commandBuffers = m_device->allocateCommandBuffers({commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY});
    if(commandBuffers.empty()) {
        qCCritical(logVulkan) << "CommandBufferManager: Cannot allocate transient command buffer";
        return TransientCommandBuffer();
    }

    TransientCommandBuffer commandBuffer;
    commandBuffer.buffer = commandBuffers[0];
    commandBuffer.parentCommandPool = commandPool;

    if(!commandBuffer.buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)) {
        qCCritical(logVulkan) << "CommandBufferManager: Cannot begin recording transient command buffer";
        m_device->freeCommandBuffers(commandPool, commandBuffers);
        return TransientCommandBuffer();
    }

    return commandBuffer;
}

bool CommandBufferManager::releaseCommandBuffer(TransientCommandBuffer &commandBuffer)
{
    if(!commandBuffer.buffer.end()) {
        qCWarning(logVulkan) << "Cannot end recoding transient command buffer";
        return false;
    }

    QMutexLocker lock(&m_mutex);
    m_executableCommandBuffers.append(commandBuffer);
    commandBuffer = {};
    return true;
}

bool CommandBufferManager::submitCommandBuffers(VkQueue queue)
{
    // TODO: Make this thread-safe once renderer is moved to a dedicated thread.

    if(m_executableCommandBuffers.size() == 0) {
        return true;
    }

    PendingCommandBuffersBatch pendingBatch;
    pendingBatch.commandsExecutedFence = m_device->createFence();
    if(!pendingBatch.commandsExecutedFence) {
        qCCritical(logVulkan) << "CommandBufferManager: Failed to submit pending command buffers";
        return false;
    }

    pendingBatch.commandBuffers.reserve(m_executableCommandBuffers.size());
    pendingBatch.parentCommandPools.reserve(m_executableCommandBuffers.size());
    for(auto &executableCommandBuffer : m_executableCommandBuffers) {
        pendingBatch.commandBuffers.append(executableCommandBuffer.buffer);
        pendingBatch.parentCommandPools.append(executableCommandBuffer.parentCommandPool);
    }

    Result submitResult;
    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.commandBufferCount = uint32_t(pendingBatch.commandBuffers.size());
    submitInfo.pCommandBuffers = pendingBatch.commandBuffers.data();
    if(VKFAILED(submitResult = vkQueueSubmit(queue, 1, &submitInfo, pendingBatch.commandsExecutedFence))) {
        qCCritical(logVulkan) << "CommandBufferManager: Failed to submit pending command buffers:" << submitResult.toString();
        m_device->destroyFence(pendingBatch.commandsExecutedFence);
        return false;
    }

    m_executableCommandBuffers.clear();
    m_pendingCommandBuffers.append(std::move(pendingBatch));
    return true;
}

void CommandBufferManager::proceedToNextFrame()
{
    // TODO: Make this thread-safe once renderer is moved to a dedicated thread.
    cleanup();
}

void CommandBufferManager::cleanup(bool freeCommandBuffers)
{
    QMutableVectorIterator<PendingCommandBuffersBatch> it(m_pendingCommandBuffers);
    while(it.hasNext()) {
        auto &pendingBatch = it.next();
        if(m_device->isFenceSignaled(pendingBatch.commandsExecutedFence)) {
            // TODO: Free command buffers in per command pool batches.
            if(freeCommandBuffers) {
                Q_ASSERT(pendingBatch.commandBuffers.size() == pendingBatch.parentCommandPools.size());
                for(int i=0; i<pendingBatch.commandBuffers.size(); ++i) {
                    m_device->freeCommandBuffer(pendingBatch.parentCommandPools[i], pendingBatch.commandBuffers[i]);
                }
            }
            m_device->destroyFence(pendingBatch.commandsExecutedFence);
            it.remove();
        }
    }
}

} // Vulkan
} // Qt3DRaytrace
