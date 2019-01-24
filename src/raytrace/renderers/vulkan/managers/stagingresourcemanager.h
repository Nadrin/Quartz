/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/vkresources.h>
#include <renderers/vulkan/commandbuffer.h>

#include <QVector>
#include <QMutex>

namespace Qt3DRaytrace {
namespace Vulkan {

class Device;

template<typename T>
struct StagingResource
{
    T resource;
    Event releaseEvent;
    void *mappedAddress = nullptr;

    template<typename DataType>
    DataType *memory() const
    {
        Q_ASSERT(mappedAddress);
        return reinterpret_cast<DataType*>(mappedAddress);
    }
    void *memory() const
    {
        return mappedAddress;
    }

    operator T() const
    {
        return resource;
    }
    operator bool() const
    {
        return resource != VK_NULL_HANDLE;
    }
};

using StagingBuffer = StagingResource<Buffer>;

class StagingResourceManager
{
public:
    explicit StagingResourceManager(Device *device);
    ~StagingResourceManager();

    StagingBuffer acquireStagingBuffer(VkDeviceSize size);
    void releaseStagingBuffer(CommandBuffer commandBuffer, StagingBuffer &buffer);

    void proceedToNextFrame();

private:
    void cleanup();

    Device *m_device = nullptr;
    QMutex m_mutex;
    QVector<StagingBuffer> m_buffers;
};

} // Vulkan
} // Qt3DRaytrace
