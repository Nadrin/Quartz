/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/resourcebarrier.h>

namespace Qt3DRaytrace {
namespace Vulkan {

enum class ResourceClass
{
    AttributeBuffer,
    IndexBuffer,
};

struct DescriptorHandle
{
    uint32_t index = 0;
    ResourceClass rclass;

    operator bool() const
    {
        return index != 0;
    }
};

struct DescriptorImageInfo : VkDescriptorImageInfo
{
    explicit DescriptorImageInfo(VkSampler sampler_)
    {
        sampler = sampler_;
        imageView = VK_NULL_HANDLE;
        imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }
    DescriptorImageInfo(VkImageView imageView_, ImageState imageState_)
    {
        sampler = VK_NULL_HANDLE;
        imageView = imageView_;
        imageLayout = ResourceBarrier::getImageLayoutFromState(imageState_);
    }
    DescriptorImageInfo(VkSampler sampler_, VkImageView imageView_, ImageState imageState_)
    {
        sampler = sampler_;
        imageView = imageView_;
        imageLayout = ResourceBarrier::getImageLayoutFromState(imageState_);
    }
};

struct DescriptorBufferInfo : VkDescriptorBufferInfo
{
    explicit DescriptorBufferInfo(VkBuffer buffer_, VkDeviceSize range_=VK_WHOLE_SIZE)
    {
        buffer = buffer_;
        offset = 0;
        range  = range_;
    }
    DescriptorBufferInfo(VkBuffer buffer_, VkDeviceSize offset_, VkDeviceSize range_)
    {
        buffer = buffer_;
        offset = offset_;
        range  = range_;
    }
};

} // Vulkan
} // Qt3DRaytrace
