/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/qvulkanrenderer_p.h>

#include <QVulkanWindow>
#include <volk.h>

#define VKFAILED(x) ((x) != VK_SUCCESS)

namespace Qt3DRaytrace {

QVulkanRendererPrivate::QVulkanRendererPrivate(QVulkanRenderer *q, QVulkanWindow *window)
    : q_ptr(q)
    , m_window(window)
    , m_frameAdvanceService(new VulkanFrameAdvanceService)
{
    Q_ASSERT(m_window);
}

QVulkanRenderer::QVulkanRenderer(QVulkanWindow *window)
    : d_ptr(new QVulkanRendererPrivate(this, window))
{}

void QVulkanRenderer::preInitResources()
{
    static const QByteArrayList RequiredDeviceExtensions {
        VK_NV_RAY_TRACING_EXTENSION_NAME,
    };

    Q_D(QVulkanRenderer);

    if(VKFAILED(volkInitialize())) {
        qFatal("QVulkanRenderer: Failed to initialize Vulkan function loader");
    }

    QVulkanInstance *instance = d->m_window->vulkanInstance();
    Q_ASSERT(instance);
    volkLoadInstance(instance->vkInstance());

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    for(int physicalDeviceIndex=0; physicalDeviceIndex < d->m_window->availablePhysicalDevices().size(); ++physicalDeviceIndex) {
        d->m_window->setPhysicalDeviceIndex(physicalDeviceIndex);
        auto deviceExtensions = d->m_window->supportedDeviceExtensions();

        bool requiredDeviceExtensionsSupported = true;
        for(const auto& requiredExtension : RequiredDeviceExtensions) {
            if(!deviceExtensions.contains(requiredExtension)) {
                requiredDeviceExtensionsSupported = false;
                break;
            }
        }
        if(requiredDeviceExtensionsSupported) {
            physicalDevice = d->m_window->physicalDevice();
            break;
        }
    }
    if(physicalDevice) {
        qInfo() << "Using physical device:" << d->m_window->physicalDeviceProperties()->deviceName;
    }
    else {
        qFatal("QVulkanRenderer: No suitable physical device found");
    }
}

void QVulkanRenderer::initResources()
{
    Q_D(QVulkanRenderer);

    d->m_device = d->m_window->device();
    Q_ASSERT(d->m_device);
    volkLoadDevice(d->m_device);
    initVulkanAllocator();

}

void QVulkanRenderer::initSwapChainResources()
{

}

void QVulkanRenderer::releaseSwapChainResources()
{

}

void QVulkanRenderer::releaseResources()
{
    releaseVulkanAllocator();
}

void QVulkanRenderer::startNextFrame()
{
    Q_D(QVulkanRenderer);



    d->m_window->frameReady();
    d->m_frameAdvanceService->proceedToNextFrame();

    d->m_window->requestUpdate();
}

QSurface *QVulkanRenderer::surface() const
{
    Q_D(const QVulkanRenderer);
    return d->m_window;
}

Qt3DCore::QAbstractFrameAdvanceService *QVulkanRenderer::frameAdvanceService() const
{
    Q_D(const QVulkanRenderer);
    return d->m_frameAdvanceService.get();
}

void QVulkanRenderer::initVulkanAllocator()
{
    Q_D(QVulkanRenderer);

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

    VmaAllocatorCreateInfo createInfo = {};
    createInfo.physicalDevice = d->m_window->physicalDevice();
    createInfo.device = d->m_window->device();
    createInfo.pVulkanFunctions = &vmaFunctions;
    if(VKFAILED(vmaCreateAllocator(&createInfo, &d->m_allocator))) {
        qFatal("QVulkanRenderer: Failed to create Vulkan memory allocator");
    }
}

void QVulkanRenderer::releaseVulkanAllocator()
{
    Q_D(QVulkanRenderer);
    if(d->m_allocator != VK_NULL_HANDLE) {
        vmaDestroyAllocator(d->m_allocator);
        d->m_allocator = VK_NULL_HANDLE;
    }
}

} // Qt3DRaytrace
