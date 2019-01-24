/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/device.h>

#include <QVulkanInstance>

namespace Qt3DRaytrace {
namespace Vulkan {

Device::~Device()
{
    if(m_allocator != VK_NULL_HANDLE) {
        vmaDestroyAllocator(m_allocator);
    }
    if(m_device != VK_NULL_HANDLE) {
        vkDestroyDevice(m_device, nullptr);
    }
}

Device *Device::create(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, const QByteArrayList &enabledExtensions)
{
    QScopedPointer<Device> device(new Device);
    device->m_physicalDevice = physicalDevice;
    device->m_queueFamilyIndex = queueFamilyIndex;

    QVector<const char*> extensions;
    extensions.reserve(enabledExtensions.size());
    for(const QByteArray &extensionName : enabledExtensions) {
        extensions.append(extensionName.data());
    }

    const float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo createInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.enabledExtensionCount = uint32_t(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    Result result;
    if(VKFAILED(result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device->m_device))) {
        qCCritical(logVulkan) << "Failed to create logical device:" << result.toString();
        return nullptr;
    }
    volkLoadDevice(device->m_device);

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
    allocatorCreateInfo.physicalDevice = device->m_physicalDevice;
    allocatorCreateInfo.device = device->m_device;
    allocatorCreateInfo.pVulkanFunctions = &vmaFunctions;
    if(VKFAILED(result = vmaCreateAllocator(&allocatorCreateInfo, &device->m_allocator))) {
        qCCritical(logVulkan) << "Failed to create Vulkan memory allocator:" << result.toString();
        return nullptr;
    }

    return device.take();
}

CommandPool Device::createCommandPool(VkCommandPoolCreateFlags createFlags)
{
    VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    createInfo.flags = createFlags;
    createInfo.queueFamilyIndex = m_queueFamilyIndex;

    CommandPool pool;
    Result result;
    if(VKFAILED(result = vkCreateCommandPool(m_device, &createInfo, nullptr, &pool.handle))) {
        qCCritical(logVulkan) << "Failed to create command pool:" << result.toString();
    }
    return pool;
}

void Device::resetCommandPool(const CommandPool &commandPool, VkCommandPoolResetFlags flags) const
{
    Result result;
    if(VKFAILED(result = vkResetCommandPool(m_device, commandPool, flags))) {
        qCCritical(logVulkan) << "Failed to reset command pool:" << result.toString();
    }
}

void Device::destroyCommandPool(CommandPool &commandPool)
{
    vkDestroyCommandPool(m_device, commandPool, nullptr);
    commandPool = {};
}

QVector<CommandBuffer> Device::allocateCommandBuffers(const CommandBufferAllocateInfo &allocInfo)
{
    QVector<CommandBuffer> commandBuffers;
    if(allocInfo.commandBufferCount == 0) {
        qCWarning(logVulkan) << "Tried to allocate zero command buffers";
        return commandBuffers;
    }

    Result result;
    commandBuffers.resize(int(allocInfo.commandBufferCount));
    if(VKFAILED(result = vkAllocateCommandBuffers(m_device, allocInfo, reinterpret_cast<VkCommandBuffer*>(commandBuffers.data())))) {
        qCCritical(logVulkan) << "Failed to allocate command buffers:" << result.toString();
    }
    return commandBuffers;
}

void Device::freeCommandBuffer(const CommandPool &commandPool, const CommandBuffer &commandBuffer)
{
    vkFreeCommandBuffers(m_device, commandPool, 1, commandBuffer);
}

void Device::freeCommandBuffers(const CommandPool &commandPool, const QVector<CommandBuffer> &commandBuffers)
{
    if(commandBuffers.size() > 0) {
        vkFreeCommandBuffers(m_device, commandPool, uint32_t(commandBuffers.size()), reinterpret_cast<const VkCommandBuffer*>(commandBuffers.data()));
    }
}

Swapchain Device::createSwapchain(QWindow *window, VkSurfaceFormatKHR format, uint32_t minImageCount, Swapchain oldSwapchain)
{
    VkSurfaceKHR surface = QVulkanInstance::surfaceForWindow(window);

    Result result;
    VkSurfaceCapabilitiesKHR surfaceCaps;
    if(VKFAILED(result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, surface, &surfaceCaps))) {
        qCCritical(logVulkan) << "Failed to query swapchain surface capabilities:" << result.toString();
        return VK_NULL_HANDLE;
    }

    Swapchain swapchain;
    VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = minImageCount;
    swapchainCreateInfo.imageFormat = format.format;
    swapchainCreateInfo.imageColorSpace = format.colorSpace;
    swapchainCreateInfo.imageExtent = surfaceCaps.currentExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.preTransform = surfaceCaps.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = oldSwapchain;
    if(VKFAILED(result = vkCreateSwapchainKHR(m_device, &swapchainCreateInfo, nullptr, &swapchain.handle))) {
        qCCritical(logVulkan) << "Failed to create swapchain:" << result.toString();
        return VK_NULL_HANDLE;
    }

    return swapchain;
}

void Device::destroySwapchain(Swapchain &swapchain)
{
    vkDestroySwapchainKHR(m_device, swapchain, nullptr);
    swapchain = {};
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

Buffer Device::createBuffer(const BufferCreateInfo &createInfo, const AllocationCreateInfo &allocInfo)
{
    Buffer buffer;

    Result result;
    if(VKFAILED(result = vmaCreateBuffer(m_allocator, createInfo, allocInfo, &buffer.handle, &buffer.allocation, nullptr))) {
        qCCritical(logVulkan) << "Failed to create buffer resource:" << result.toString();
    }
    return buffer;
}

void Device::destroyBuffer(Buffer &buffer)
{
    vmaDestroyBuffer(m_allocator, buffer.handle, buffer.allocation);
    buffer = {};
}

VkImageView Device::createImageView(const ImageViewCreateInfo &createInfo)
{
    VkImageView imageView = VK_NULL_HANDLE;

    Result result;
    if(VKFAILED(result = vkCreateImageView(m_device, &createInfo, nullptr, &imageView))) {
        qCCritical(logVulkan) << "Failed to create image view:" << result.toString();
    }
    return imageView;
}

void Device::destroyImageView(VkImageView &imageView)
{
    vkDestroyImageView(m_device, imageView, nullptr);
    imageView = VK_NULL_HANDLE;
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

void Device::resetDescriptorPool(const DescriptorPool &descriptorPool) const
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

Semaphore Device::createSemaphore()
{
    VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

    Semaphore semaphore;
    Result result;
    if(VKFAILED(result = vkCreateSemaphore(m_device, &createInfo, nullptr, &semaphore.handle))) {
        qCCritical(logVulkan) << "Failed to create semaphore:" << result.toString();
    }
    return semaphore;
}

void Device::destroySemaphore(Semaphore &semaphore)
{
    vkDestroySemaphore(m_device, semaphore, nullptr);
    semaphore = {};
}

Fence Device::createFence(VkFenceCreateFlags flags)
{
    VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    createInfo.flags = flags;

    Fence fence;
    Result result;
    if(VKFAILED(result = vkCreateFence(m_device, &createInfo, nullptr, &fence.handle))) {
        qCCritical(logVulkan) << "Failed to create fence:" << result.toString();
    }
    return fence;
}

bool Device::waitForFence(const Fence &fence, uint64_t timeout) const
{
    Result result = vkWaitForFences(m_device, 1, &fence.handle, VK_TRUE, timeout);
    if(VKFAILED(result) && result != VK_TIMEOUT) {
        qCCritical(logVulkan) << "Failed to wait for fence:" << result.toString();
    }
    return result != VK_TIMEOUT;
}

bool Device::isFenceSignaled(const Fence &fence) const
{
    Result result = vkGetFenceStatus(m_device, fence);
    return result == VK_SUCCESS;
}

Result Device::resetFence(const Fence &fence) const
{
    Result result;
    if(VKFAILED(result = vkResetFences(m_device, 1, &fence.handle))) {
        qCCritical(logVulkan) << "Failed to reset fence:" << result.toString();
    }
    return result;
}

void Device::destroyFence(Fence &fence)
{
    vkDestroyFence(m_device, fence, nullptr);
    fence = {};
}

Event Device::createEvent()
{
    Event event;
    VkEventCreateInfo createInfo = { VK_STRUCTURE_TYPE_EVENT_CREATE_INFO };
    Result result;
    if(VKFAILED(vkCreateEvent(m_device, &createInfo, nullptr, &event.handle))) {
        qCCritical(logVulkan) << "Failed to create event:" << result.toString();
    }
    return event;
}

void Device::destroyEvent(Event &event)
{
    vkDestroyEvent(m_device, event, nullptr);
    event = {};
}

bool Device::isEventSignaled(const Event &event) const
{
    Result result = vkGetEventStatus(m_device, event);
    return result == VK_EVENT_SET;
}

RenderPass Device::createRenderPass(const RenderPassCreateInfo &createInfo)
{
    RenderPass renderPass;
    Result result;
    if(VKFAILED(result = vkCreateRenderPass(m_device, createInfo, nullptr, &renderPass.handle))) {
        qCCritical(logVulkan) << "Failed to create render pass:" << result.toString();
    }
    return renderPass;
}

void Device::destroyRenderPass(RenderPass &renderPass)
{
    vkDestroyRenderPass(m_device, renderPass, nullptr);
    renderPass = {};
}

Framebuffer Device::createFramebuffer(const FramebufferCreateInfo &createInfo)
{
    Framebuffer fb;
    Result result;
    if(VKFAILED(result = vkCreateFramebuffer(m_device, createInfo, nullptr, &fb.handle))) {
        qCCritical(logVulkan) << "Failed to create framebuffer:" << result.toString();
    }
    return fb;
}

void Device::destroyFramebuffer(Framebuffer &framebuffer)
{
    vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    framebuffer = {};
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

void *Device::mapMemory(const VmaAllocation &allocation) const
{
    void *mappedAddress;
    Result result;
    if(VKFAILED(result = vmaMapMemory(m_allocator, allocation, &mappedAddress))) {
        qCCritical(logVulkan) << "Failed to map resource memory:" << result.toString();
        return nullptr;
    }
    return mappedAddress;
}

void Device::unmapMemory(const VmaAllocation &allocation) const
{
    vmaUnmapMemory(m_allocator, allocation);
}

void Device::waitIdle() const
{
    vkDeviceWaitIdle(m_device);
}

bool Device::isValid() const
{
    return m_device != VK_NULL_HANDLE && m_allocator != VK_NULL_HANDLE;
}

} // Vulkan
} // Qt3DRaytrace
