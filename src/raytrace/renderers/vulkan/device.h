/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/vkresources.h>
#include <renderers/vulkan/initializers.h>
#include <renderers/vulkan/pipeline/pipeline.h>
#include <renderers/vulkan/commandbuffer.h>
#include <renderers/vulkan/geometry.h>

#include <QMutex>
#include <QVector>

class QWindow;

namespace Qt3DRaytrace {
namespace Vulkan {

class Device
{
public:
    ~Device();

    enum class ScratchBufferType {
        Build,
        Update,
    };

    static Device *create(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, const QByteArrayList &enabledExtensions={});

    CommandPool createCommandPool(VkCommandPoolCreateFlags createFlags=0);
    void resetCommandPool(const CommandPool &commandPool, VkCommandPoolResetFlags flags=0) const;
    void destroyCommandPool(CommandPool &commandPool);

    QVector<CommandBuffer> allocateCommandBuffers(const CommandBufferAllocateInfo &allocInfo);
    void freeCommandBuffer(const CommandPool &commandPool, const CommandBuffer &commandBuffer);
    void freeCommandBuffers(const CommandPool &commandPool, const QVector<CommandBuffer> &commandBuffers);

    Swapchain createSwapchain(QWindow *window, VkSurfaceFormatKHR format, VkPresentModeKHR presentMode, uint32_t minImageCount, Swapchain oldSwapchain);
    void destroySwapchain(Swapchain& swapchain);

    Image createImage(const ImageCreateInfo &createInfo, const AllocationCreateInfo &allocCreateInfo);
    Image createStagingImage(uint32_t width, uint32_t height, VkFormat format);
    void destroyImage(Image &image);

    Buffer createBuffer(const BufferCreateInfo &createInfo, const AllocationCreateInfo &allocCreateInfo);
    Buffer createStagingBuffer(VkDeviceSize size);
    void destroyBuffer(Buffer &buffer);

    VkImageView createImageView(const ImageViewCreateInfo &createInfo);
    void destroyImageView(VkImageView &imageView);

    AccelerationStructure createAccelerationStructure(const AccelerationStructureCreateInfo &createInfo);
    Buffer createAccelerationStructureScratchBuffer(const AccelerationStructure &as, ScratchBufferType type);
    void destroyAccelerationStructure(AccelerationStructure &as);

    DescriptorPool createDescriptorPool(const DescriptorPoolCreateInfo &createInfo);
    void resetDescriptorPool(const DescriptorPool &descriptorPool) const;
    void destroyDescriptorPool(DescriptorPool &descriptorPool);

    QVector<DescriptorSet> allocateDescriptorSets(const DescriptorSetAllocateInfo &allocInfo);
    void writeDescriptor(const WriteDescriptorSet &writeOp);
    void writeDescriptor(const WriteDescriptorSet &writeOp, const AccelerationStructure &as);
    void writeDescriptors(const QVector<WriteDescriptorSet> &writeOps);

    QueryPool createQueryPool(const QueryPoolCreateInfo &createInfo);
    void destroyQueryPool(QueryPool &queryPool);

    Sampler createSampler(const SamplerCreateInfo &createInfo);
    void destroySampler(Sampler &sampler);

    Semaphore createSemaphore();
    void destroySemaphore(Semaphore &semaphore);

    Fence createFence(VkFenceCreateFlags flags=0);
    bool waitForFence(const Fence &fence, uint64_t timeout=UINT64_MAX) const;
    bool isFenceSignaled(const Fence &fence) const;
    Result resetFence(const Fence &fence) const;
    void destroyFence(Fence &fence);

    Event createEvent();
    void destroyEvent(Event &event);
    bool isEventSignaled(const Event &event) const;

    RenderPass createRenderPass(const RenderPassCreateInfo &createInfo);
    void destroyRenderPass(RenderPass &renderPass);

    Framebuffer createFramebuffer(const FramebufferCreateInfo &createInfo);
    void destroyFramebuffer(Framebuffer &framebuffer);

    void destroyPipeline(Pipeline &pipeline);
    void destroyGeometry(Geometry &geometry);

    void *mapMemory(const VmaAllocation &allocation) const;
    void unmapMemory(const VmaAllocation &allocation) const;

    bool queryTimeElapsed(const QueryPool &queryPool, uint32_t firstTimestampQueryIndex, double &msElapsed, VkQueryResultFlags flags=0) const;

    void waitIdle() const;

    VkDevice device() const { return m_device; }
    operator VkDevice() const { return m_device; }

    VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
    VmaAllocator allocator() const { return m_allocator; }
    uint32_t queueFamilyIndex() const { return m_queueFamilyIndex; }

    const VkPhysicalDeviceProperties &physicalDeviceProperties() const { return m_physicalDeviceProperties; }
    const VkPhysicalDeviceRayTracingPropertiesNV &rayTracingProperties() const { return m_rayTracingProperties; }

    bool isValid() const;

    Q_DISABLE_COPY(Device)
private:
    Device() = default;
    void queryPhysicalDeviceProperties();

    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VmaAllocator m_allocator = VK_NULL_HANDLE;
    uint32_t m_queueFamilyIndex = uint32_t(-1);

    VmaPool m_accelerationStructuresPool = VK_NULL_HANDLE;
    QMutex m_accelerationStructuresPoolMutex;

    VkPhysicalDeviceProperties m_physicalDeviceProperties;
    VkPhysicalDeviceRayTracingPropertiesNV m_rayTracingProperties;
};

} // Vulkan
} // Qt3DRaytrace
