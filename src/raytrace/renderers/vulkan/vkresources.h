/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>

namespace Qt3DRaytrace {
namespace Vulkan {

template<typename T>
struct Resource
{
    Resource(T handle = VK_NULL_HANDLE)
        : handle(handle)
    {}

    operator T() const { return handle; }
    operator T*() { return &handle; }
    operator const T*() const { return &handle; }
    operator bool() const { return isValid(); }

    bool isValid() const
    {
        return handle != VK_NULL_HANDLE;
    }

    T handle;
};

template<typename T>
struct MemoryResource : Resource<T>
{
    MemoryResource(T handle = VK_NULL_HANDLE)
        : Resource<T>(handle)
        , allocation(VK_NULL_HANDLE)
        , hostAddress(nullptr)
    {}

    bool isAllocated() const
    {
        return allocation != VK_NULL_HANDLE;
    }
    bool isHostAccessible() const
    {
        return hostAddress != nullptr;
    }

    void *memory() const
    {
        Q_ASSERT(hostAddress);
        return hostAddress;
    }
    template<typename U> U *memory() const
    {
        Q_ASSERT(hostAddress);
        return reinterpret_cast<U*>(hostAddress);
    }

    VmaAllocation allocation;
    void *hostAddress;
};

template<typename T>
struct RetiredResource
{
    explicit RetiredResource(T resource=T(), uint32_t initialTTL=0)
        : resource(resource)
        , ttl(initialTTL)
    {}

    bool updateTTL()
    {
        Q_ASSERT(ttl > 0);
        --ttl;
        return ttl > 0;
    }

    T resource;
    uint32_t ttl;
};

struct Image : MemoryResource<VkImage>
{
    VkImageView view = VK_NULL_HANDLE;
};

struct Buffer : MemoryResource<VkBuffer>
{};

struct AccelerationStructure : MemoryResource<VkAccelerationStructureNV>
{};

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


} // Vulkan
} // Qt3DRaytrace
