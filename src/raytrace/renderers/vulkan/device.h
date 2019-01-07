/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/initializers.h>
#include <renderers/vulkan/pipeline/pipeline.h>

#include <QVector>

namespace Qt3DRaytrace {
namespace Vulkan {

struct Image : MemoryResource<VkImage>
{
    VkImageView view = VK_NULL_HANDLE;
};

using DescriptorPool = Resource<VkDescriptorPool>;
using DescriptorSet = Resource<VkDescriptorSet>;
using QueryPool = Resource<VkQueryPool>;
using Sampler = Resource<VkSampler>;

class Device
{
public:
    Device(VkDevice device, VkPhysicalDevice physicalDevice);
    ~Device();

    Image createImage(const ImageCreateInfo &createInfo, const AllocationCreateInfo &allocInfo);
    void destroyImage(Image& image);

    DescriptorPool createDescriptorPool(const DescriptorPoolCreateInfo &createInfo);
    void resetDescriptorPool(const DescriptorPool &descriptorPool);
    void destroyDescriptorPool(DescriptorPool &descriptorPool);

    QVector<DescriptorSet> allocateDescriptorSets(const DescriptorSetAllocateInfo &allocInfo);
    void writeDescriptorSets(const QVector<WriteDescriptorSet> &writeOperations);

    QueryPool createQueryPool(const QueryPoolCreateInfo &createInfo);
    void destroyQueryPool(QueryPool &queryPool);

    Sampler createSampler(const SamplerCreateInfo &createInfo);
    void destroySampler(Sampler &sampler);

    void destroyPipeline(Pipeline &pipeline);

    VkDevice device() const { return m_device; }
    operator VkDevice() const { return m_device; }

    VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
    VmaAllocator allocator() const { return m_allocator; }

    bool isValid() const;

    Q_DISABLE_COPY(Device)
private:
    VkDevice m_device;
    VkPhysicalDevice m_physicalDevice;
    VmaAllocator m_allocator;

};

} // Vulkan
} // Qt3DRaytrace
