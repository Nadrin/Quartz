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
    explicit CommandBufferManager(Renderer *renderer);
    ~CommandBufferManager();

    TransientCommandBuffer acquireCommandBuffer();
    bool releaseCommandBuffer(TransientCommandBuffer &commandBuffer, const QVector<Buffer> &transientBuffers);
    bool releaseCommandBuffer(TransientCommandBuffer &commandBuffer, const QVector<Image>  &transientImages);
    bool releaseCommandBuffer(TransientCommandBuffer &commandBuffer, const QVector<Buffer> &transientBuffers, const QVector<Image> &transientImages);

    bool submitCommandBuffers(VkQueue queue);
    void destroyExpiredResources();
    void proceedToNextFrame();

    Q_DISABLE_COPY(CommandBufferManager)

private:
    void cleanup(bool freeCommandBuffers=true);

    Device *m_device;

    struct ExecutableCommandBuffer {
        TransientCommandBuffer commandBuffer;
        QVector<Buffer> transientBuffers;
        QVector<Image> transientImages;
    };
    struct PendingCommandBuffersBatch {
        QVector<VkCommandBuffer> commandBuffers;
        QVector<VkCommandPool> parentCommandPools;
        QVector<Buffer> transientBuffers;
        QVector<Image> transientImages;
        Fence commandsExecutedFence;
    };

    QMutex m_commandBuffersMutex;
    QVector<ExecutableCommandBuffer> m_executableCommandBuffers;
    QVector<PendingCommandBuffersBatch> m_pendingCommandBuffers;

    QVector<CommandPool> m_commandPools;
    QThreadStorage<CommandPool> m_localCommandPool;

    QMutex m_retiredResourcesMutex;
    QVector<Buffer> m_retiredBuffers;
    QVector<Image> m_retiredImages;
};

} // Vulkan
} // Qt3DRaytrace
