/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/pipeline/pipeline.h>
#include <renderers/vulkan/resourcebarrier.h>

#include <QVector>

namespace Qt3DRaytrace {
namespace Vulkan {

class CommandBuffer : public Resource<VkCommandBuffer>
{
public:
    CommandBuffer(VkCommandBuffer commandBuffer);

    void clearColorImage(VkImage image, ImageState imageState, const ImageSubresourceRange &range={});

    void resourceBarrier(int numBufferTransitions, const BufferTransition *bufferTransitions, int numImageTransitions, const ImageTransition *imageTransitions) const;
    void resourceBarrier(const QVector<BufferTransition> &bufferTransitions, const QVector<ImageTransition> &imageTransitions) const;
    void resourceBarrier(const QVector<BufferTransition> &bufferTransitions) const;
    void resourceBarrier(const QVector<ImageTransition> &imageTransitions) const;

    void resourceBarrier(const BufferTransition &bufferTransition) const
    {
        resourceBarrier(1, &bufferTransition, 0, nullptr);
    }
    void resourceBarrier(const ImageTransition &imageTransition) const
    {
        resourceBarrier(0, nullptr, 1, &imageTransition);
    }
};

} // Vulkan
} // Qt3DRaytrace
