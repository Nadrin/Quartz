/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>

namespace Qt3DRaytrace {
namespace Vulkan {

using CommandPool = Resource<VkCommandPool>;
using DescriptorPool = Resource<VkDescriptorPool>;
using QueryPool = Resource<VkQueryPool>;

using DescriptorSet = Resource<VkDescriptorSet>;

using Sampler = Resource<VkSampler>;

using Swapchain = Resource<VkSwapchainKHR>;
using Semaphore = Resource<VkSemaphore>;
using Fence = Resource<VkFence>;
using Event = Resource<VkEvent>;

using Framebuffer = Resource<VkFramebuffer>;
using RenderPass = Resource<VkRenderPass>;

struct Image : MemoryResource<VkImage>
{
    VkImageView view = VK_NULL_HANDLE;
};

struct Buffer : MemoryResource<VkBuffer>
{};

struct AccelerationStructure : MemoryResource<VkAccelerationStructureNV>
{};

} // Vulkan
} // Qt3DRaytrace
