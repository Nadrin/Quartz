/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <QVarLengthArray>

namespace Qt3DRaytrace {
namespace Vulkan {

template<typename T>
struct Resource
{
    Resource(T handle = VK_NULL_HANDLE)
        : handle(handle)
    {}

    operator T() const { return handle; }
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
class ManagedResource
{
public:
    void update(const T &newResource, uint32_t retireTTL)
    {
        if(resource) {
            RetiredResource retiredResouce;
            retiredResouce.resource = resource;
            retiredResouce.ttl = retireTTL;
            m_retired.append(retiredResouce);
        }
        resource = newResource;
    }

    void updateRetiredTTL()
    {
        for(auto &retired : m_retired) {
            --retired.ttl;
        }
    }

    QVarLengthArray<T> retired() const
    {
        QVarLengthArray<T> result;
        result.reserve(m_retired.size());
        for(const auto &retiredResource : m_retired) {
            result.append(retiredResource.resource);
        }
        return result;
    }

    QVarLengthArray<T> takeExpired()
    {
        QVarLengthArray<T> expired;
        for(int i=0; i<m_retired.size();) {
            if(m_retired[i].ttl <= 0) {
                expired.append(m_retired[i].resource);
                m_retired.remove(i);
            }
            else {
                ++i;
            }
        }
        return expired;
    }

    void reset(const T &newResource={})
    {
        resource = newResource;
        m_retired.clear();
    }

    T resource;

private:
    struct RetiredResource {
        T resource;
        int ttl;
    };
    QVarLengthArray<RetiredResource> m_retired;
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
