/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/vkresources.h>
#include <renderers/vulkan/commandbuffer.h>

#include <QThreadStorage>
#include <QVector>
#include <QMutex>

namespace Qt3DRaytrace {
namespace Vulkan {

class Renderer;
class Device;

struct TransientCommandBuffer
{
    CommandBuffer buffer;
    CommandPool parentCommandPool;

    operator bool() const
    {
        return buffer != VK_NULL_HANDLE;
    }
    operator VkCommandBuffer() const
    {
        return buffer.handle;
    }
    operator CommandBuffer() const
    {
        return buffer;
    }
    const CommandBuffer *operator->() const
    {
        return &buffer;
    }
};

class CommandBufferManager
{
public:
    explicit CommandBufferManager(Device *device);
    ~CommandBufferManager();

    TransientCommandBuffer acquireCommandBuffer();
    bool releaseCommandBuffer(TransientCommandBuffer &commandBuffer);

    bool submitCommandBuffers(VkQueue queue);
    void proceedToNextFrame();

private:
    void cleanup(bool freeCommandBuffers=true);

    Device *m_device;

    struct PendingCommandBuffersBatch {
        QVector<VkCommandBuffer> commandBuffers;
        QVector<VkCommandPool> parentCommandPools;
        Fence commandsExecutedFence;
    };

    QMutex m_mutex;
    QVector<TransientCommandBuffer> m_executableCommandBuffers;
    QVector<PendingCommandBuffersBatch> m_pendingCommandBuffers;
    QVector<CommandPool> m_commandPools;
    QThreadStorage<CommandPool> m_localCommandPool;
};

} // Vulkan
} // Qt3DRaytrace
