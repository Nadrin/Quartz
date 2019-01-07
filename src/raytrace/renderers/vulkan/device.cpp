/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/device.h>

namespace Qt3DRaytrace {
namespace Vulkan {

Device::Device(VkDevice device, VkPhysicalDevice physicalDevice)
    : m_device(device)
    , m_physicalDevice(physicalDevice)
    , m_allocator(VK_NULL_HANDLE)
{
    Q_ASSERT(m_device != VK_NULL_HANDLE);
    Q_ASSERT(m_physicalDevice != VK_NULL_HANDLE);

    volkLoadDevice(m_device);

    VmaVulkanFunctions vmaFunctions;
    vmaFunctions.vkAllocateMemory = vkAllocateMemory;
    vmaFunctions.vkBindBufferMemory = vkBindBufferMemory;
    vmaFunctions.vkBindImageMemory = vkBindImageMemory;
    vmaFunctions.vkCreateBuffer = vkCreateBuffer;
    vmaFunctions.vkCreateImage = vkCreateImage;
    vmaFunctions.vkDestroyBuffer = vkDestroyBuffer;
    vmaFunctions.vkDestroyImage = vkDestroyImage;
    vmaFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    vmaFunctions.vkFreeMemory = vkFreeMemory;
    vmaFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    vmaFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    vmaFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    vmaFunctions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
    vmaFunctions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
    vmaFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    vmaFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    vmaFunctions.vkMapMemory = vkMapMemory;
    vmaFunctions.vkUnmapMemory = vkUnmapMemory;

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.physicalDevice = m_physicalDevice;
    allocatorCreateInfo.device = m_device;
    allocatorCreateInfo.pVulkanFunctions = &vmaFunctions;

    Result result;
    if(VKFAILED(result = vmaCreateAllocator(&allocatorCreateInfo, &m_allocator))) {
        qCCritical(logVulkan) << "Failed to create Vulkan memory allocator:" << result.toString();
    }
}

Device::~Device()
{
    if(m_allocator != VK_NULL_HANDLE) {
        vmaDestroyAllocator(m_allocator);
    }
}

Image Device::createImage(const ImageCreateInfo &createInfo, const AllocationCreateInfo &allocInfo)
{
    Image image;

    Result result;
    if(VKFAILED(result = vmaCreateImage(m_allocator, &createInfo, &allocInfo, &image.handle, &image.allocation, nullptr))) {
        qCCritical(logVulkan) << "Failed to create image resource:" << result.toString();
        return image;
    }

    VkImageViewType viewType;
    switch(createInfo.imageType) {
    case VK_IMAGE_TYPE_1D:
        viewType = VK_IMAGE_VIEW_TYPE_1D;
        break;
    case VK_IMAGE_TYPE_2D:
        viewType = VK_IMAGE_VIEW_TYPE_2D;
        break;
    case VK_IMAGE_TYPE_3D:
        viewType = VK_IMAGE_VIEW_TYPE_3D;
        break;
    default:
        Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid image type");
    }

    VkImageAspectFlags aspectFlags;
    switch(createInfo.format) {
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_X8_D24_UNORM_PACK32:
        aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
        break;
    case VK_FORMAT_D16_UNORM_S8_UINT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        break;
    default:
        aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    Vulkan::ImageViewCreateInfo viewCreateInfo(image.handle, viewType, createInfo.format, aspectFlags);
    if(VKFAILED(result = vkCreateImageView(m_device, &viewCreateInfo, nullptr, &image.view))) {
        qCCritical(logVulkan) << "Failed to create image view:" << result.toString();
    }
    return image;
}

void Device::destroyImage(Image &image)
{
    vkDestroyImageView(m_device, image.view, nullptr);
    vmaDestroyImage(m_allocator, image.handle, image.allocation);
    image = {};
}

DescriptorPool Device::createDescriptorPool(const DescriptorPoolCreateInfo &createInfo)
{
    DescriptorPool pool;
    Result result;
    if(VKFAILED(result = vkCreateDescriptorPool(m_device, &createInfo, nullptr, &pool.handle))) {
        qCCritical(logVulkan) << "Failed to create descriptor pool:" << result.toString();
    }
    return pool;
}

void Device::resetDescriptorPool(const DescriptorPool &descriptorPool)
{
    Result result;
    if(VKFAILED(result = vkResetDescriptorPool(m_device, descriptorPool.handle, 0))) {
        qCWarning(logVulkan) << "Failed to reset descriptor pool:" << result.toString();
    }
}

void Device::destroyDescriptorPool(DescriptorPool &descriptorPool)
{
    vkDestroyDescriptorPool(m_device, descriptorPool.handle, nullptr);
    descriptorPool = {};
}

QVector<DescriptorSet> Device::allocateDescriptorSets(const DescriptorSetAllocateInfo &allocInfo)
{
    QVector<DescriptorSet> descriptorSets;
    if(allocInfo.descriptorSetCount == 0) {
        qCWarning(logVulkan) << "Tried to allocate zero descriptor sets";
        return descriptorSets;
    }

    Result result;
    descriptorSets.resize(int(allocInfo.descriptorSetCount));
    if(VKFAILED(result = vkAllocateDescriptorSets(m_device, &allocInfo, reinterpret_cast<VkDescriptorSet*>(descriptorSets.data())))) {
        qCCritical(logVulkan) << "Failed to allocate descriptor sets:" << result.toString();
        descriptorSets.clear();
    }
    return descriptorSets;
}

void Device::writeDescriptorSets(const QVector<WriteDescriptorSet> &writeOperations)
{
    if(writeOperations.size() > 0) {
        vkUpdateDescriptorSets(m_device, uint32_t(writeOperations.size()), writeOperations.data(), 0, nullptr);
    }
}

QueryPool Device::createQueryPool(const QueryPoolCreateInfo &createInfo)
{
    QueryPool pool;
    Result result;
    if(VKFAILED(result = vkCreateQueryPool(m_device, &createInfo, nullptr, &pool.handle))) {
        qCCritical(logVulkan) << "Failed to create query pool:" << result.toString();
    }
    return pool;
}

void Device::destroyQueryPool(QueryPool &queryPool)
{
    vkDestroyQueryPool(m_device, queryPool.handle, nullptr);
    queryPool = {};
}

Sampler Device::createSampler(const SamplerCreateInfo &createInfo)
{
    Sampler sampler;
    Result result;
    if(VKFAILED(result = vkCreateSampler(m_device, &createInfo, nullptr, &sampler.handle))) {
        qCCritical(logVulkan) << "Failed to create sampler:" << result.toString();
    }
    return sampler;
}

void Device::destroySampler(Sampler &sampler)
{
    vkDestroySampler(m_device, sampler.handle, nullptr);
    sampler = {};
}

void Device::destroyPipeline(Pipeline &pipeline)
{
    vkDestroyPipeline(m_device, pipeline.handle, nullptr);
    vkDestroyPipelineLayout(m_device, pipeline.pipelineLayout, nullptr);
    for(VkDescriptorSetLayout setLayout : pipeline.descriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(m_device, setLayout, nullptr);
    }
    pipeline = Pipeline{};
}

bool Device::isValid() const
{
    return m_device != VK_NULL_HANDLE && m_allocator != VK_NULL_HANDLE;
}

} // Vulkan
} // Qt3DRaytrace
