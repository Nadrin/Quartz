/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/initializers.h>
#include <renderers/vulkan/pipeline/pipeline.h>
#include <renderers/vulkan/commandbuffer.h>

#include <QVector>

class QWindow;

namespace Qt3DRaytrace {
namespace Vulkan {

struct Image : MemoryResource<VkImage>
{
    VkImageView view = VK_NULL_HANDLE;
};

using CommandPool = Resource<VkCommandPool>;
using DescriptorPool = Resource<VkDescriptorPool>;
using DescriptorSet = Resource<VkDescriptorSet>;
using QueryPool = Resource<VkQueryPool>;
using Sampler = Resource<VkSampler>;
using Swapchain = Resource<VkSwapchainKHR>;
using Semaphore = Resource<VkSemaphore>;
using Fence = Resource<VkFence>;
using Framebuffer = Resource<VkFramebuffer>;
using RenderPass = Resource<VkRenderPass>;

class Device
{
public:
    ~Device();

    static Device *create(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, const QByteArrayList &enabledExtensions={});

    CommandPool createCommandPool(VkCommandPoolCreateFlags createFlags=0);
    void resetCommandPool(const CommandPool &commandPool, VkCommandPoolResetFlags flags=0) const;
    void destroyCommandPool(CommandPool &commandPool);

    QVector<CommandBuffer> allocateCommandBuffers(const CommandBufferAllocateInfo &allocInfo);
    void freeCommandBuffers(const CommandPool &commandPool, const QVector<CommandBuffer> &commandBuffers);

    Swapchain createSwapchain(QWindow *window, VkSurfaceFormatKHR format, uint32_t minImageCount, Swapchain oldSwapchain);
    void destroySwapchain(Swapchain& swapchain);

    Image createImage(const ImageCreateInfo &createInfo, const AllocationCreateInfo &allocInfo);
    void destroyImage(Image &image);

    VkImageView createImageView(const ImageViewCreateInfo &createInfo);
    void destroyImageView(VkImageView &imageView);

    DescriptorPool createDescriptorPool(const DescriptorPoolCreateInfo &createInfo);
    void resetDescriptorPool(const DescriptorPool &descriptorPool) const;
    void destroyDescriptorPool(DescriptorPool &descriptorPool);

    QVector<DescriptorSet> allocateDescriptorSets(const DescriptorSetAllocateInfo &allocInfo);
    void writeDescriptorSets(const QVector<WriteDescriptorSet> &writeOperations);

    QueryPool createQueryPool(const QueryPoolCreateInfo &createInfo);
    void destroyQueryPool(QueryPool &queryPool);

    Sampler createSampler(const SamplerCreateInfo &createInfo);
    void destroySampler(Sampler &sampler);

    Semaphore createSemaphore();
    void destroySemaphore(Semaphore &semaphore);

    Fence createFence(VkFenceCreateFlags flags=0);
    bool waitForFence(const Fence &fence, uint64_t timeout=UINT64_MAX) const;
    Result resetFence(const Fence &fence) const;
    void destroyFence(Fence &fence);

    RenderPass createRenderPass(const RenderPassCreateInfo &createInfo);
    void destroyRenderPass(RenderPass &renderPass);

    Framebuffer createFramebuffer(const FramebufferCreateInfo &createInfo);
    void destroyFramebuffer(Framebuffer &framebuffer);

    void destroyPipeline(Pipeline &pipeline);

    void waitIdle() const;

    VkDevice device() const { return m_device; }
    operator VkDevice() const { return m_device; }

    VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
    VmaAllocator allocator() const { return m_allocator; }
    uint32_t queueFamilyIndex() const { return m_queueFamilyIndex; }

    bool isValid() const;

    Q_DISABLE_COPY(Device)
private:
    Device() = default;

    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VmaAllocator m_allocator = VK_NULL_HANDLE;
    uint32_t m_queueFamilyIndex = uint32_t(-1);
};

} // Vulkan
} // Qt3DRaytrace
