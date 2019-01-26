/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/renderer.h>
#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/commandbuffer.h>
#include <renderers/vulkan/initializers.h>
#include <renderers/vulkan/shadermodule.h>
#include <renderers/vulkan/pipeline/graphicspipeline.h>
#include <renderers/vulkan/pipeline/computepipeline.h>

#include <backend/managers_p.h>

#include <QVulkanInstance>
#include <QWindow>
#include <QTimer>

#include <QtMath>

namespace Qt3DRaytrace {
namespace Vulkan {

Q_LOGGING_CATEGORY(logVulkan, "raytrace.vulkan")

Renderer::Renderer(QObject *parent)
    : QObject(parent)
    , m_renderTimer(new QTimer(this))
    , m_frameAdvanceService(new FrameAdvanceService)
    , m_updateWorldTransformJob(new Raytrace::UpdateWorldTransformJob)
    , m_destroyExpiredResourcesJob(new DestroyExpiredResourcesJob(this))
{
    QObject::connect(m_renderTimer, &QTimer::timeout, this, &Renderer::renderFrame);
}

bool Renderer::initialize()
{
    static const QByteArrayList RequiredDeviceExtensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_NV_RAY_TRACING_EXTENSION_NAME,
    };

    if(!m_window) {
        qCCritical(logVulkan) << "Cannot initialize renderer: no surface set";
        return false;
    }
    if(!m_window->vulkanInstance()) {
        qCCritical(logVulkan) << "Cannot initialize renderer: no Vulkan instance set";
        return false;
    }

    if(VKFAILED(volkInitialize())) {
        qCCritical(logVulkan) << "Failed to initialize Vulkan function loader";
        return false;
    }

    m_instance = m_window->vulkanInstance();
    volkLoadInstance(m_instance->vkInstance());

    uint32_t queueFamilyIndex;
    VkPhysicalDevice physicalDevice = choosePhysicalDevice(RequiredDeviceExtensions, queueFamilyIndex);
    if(physicalDevice == VK_NULL_HANDLE) {
        qCCritical(logVulkan) << "No suitable Vulkan physical device found";
        return false;
    }

    m_device = QSharedPointer<Device>(Device::create(physicalDevice, queueFamilyIndex, RequiredDeviceExtensions));
    if(!m_device) {
        return false;
    }
    vkGetDeviceQueue(*m_device, queueFamilyIndex, 0, &m_graphicsQueue);

    int numConcurrentFrames;
    if(!querySwapchainProperties(physicalDevice, m_swapchainFormat, numConcurrentFrames)) {
        return false;
    }
    m_frameResources.resize(numConcurrentFrames);

    m_commandBufferManager.reset(new CommandBufferManager(m_device.get()));

    if(!createResources()) {
        return false;
    }

    m_renderTimer->start();
    m_frameAdvanceService->proceedToNextFrame();
    return true;
}

void Renderer::shutdown()
{
    m_renderTimer->stop();

    if(m_device) {
        m_device->waitIdle();

        m_commandBufferManager.reset();

        releaseSwapchainResources();
        m_device->destroySwapchain(m_swapchain);
        releaseResources();

        m_device.reset();
    }

    m_swapchain = {};
    m_graphicsQueue = VK_NULL_HANDLE;
}

bool Renderer::createResources()
{
    m_renderingFinishedSemaphore = m_device->createSemaphore();
    m_presentationFinishedSemaphore = m_device->createSemaphore();

    m_displayRenderPass = createDisplayRenderPass(m_swapchainFormat.format);

    m_staticCommandPool = m_device->createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    m_queryPool = m_device->createQueryPool({VK_QUERY_TYPE_TIMESTAMP, 2 * numConcurrentFrames()});

    m_defaultSampler = m_device->createSampler({VK_FILTER_NEAREST});

    {
        QVector<CommandBuffer> frameCommandBuffers = m_device->allocateCommandBuffers({m_staticCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, uint32_t(numConcurrentFrames())});
        for(int i=0; i<int(numConcurrentFrames()); ++i) {
            m_frameResources[i].commandBuffer = frameCommandBuffers[i];
            m_frameResources[i].commandBuffersExecutedFence = m_device->createFence(VK_FENCE_CREATE_SIGNALED_BIT);
        }
    }

    {
        const QVector<VkDescriptorPoolSize> descriptorPoolSizes = {
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, numConcurrentFrames() },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, numConcurrentFrames() },
        };
        m_swapchainDescriptorPool = m_device->createDescriptorPool({ 2 * numConcurrentFrames(), descriptorPoolSizes});
    }

    m_displayPipeline = GraphicsPipelineBuilder(m_device.get(), m_displayRenderPass)
            .shaders({"display.vert", "display.frag"})
            .defaultSampler(m_defaultSampler)
            .build();

    m_testPipeline = ComputePipelineBuilder(m_device.get())
            .shaders({"test.comp"})
            .build();

    return true;
}

void Renderer::releaseResources()
{
    m_device->destroySemaphore(m_renderingFinishedSemaphore);
    m_device->destroySemaphore(m_presentationFinishedSemaphore);

    m_device->destroyRenderPass(m_displayRenderPass);

    m_device->destroyCommandPool(m_staticCommandPool);
    m_device->destroyQueryPool(m_queryPool);

    m_device->destroySampler(m_defaultSampler);

    m_device->destroyDescriptorPool(m_swapchainDescriptorPool);

    m_device->destroyPipeline(m_displayPipeline);
    m_device->destroyPipeline(m_testPipeline);

    for(auto &frame : m_frameResources) {
        m_device->destroyFence(frame.commandBuffersExecutedFence);
    }
}

void Renderer::createSwapchainResources()
{
    Result result;

    const uint32_t swapchainWidth = uint32_t(m_swapchainSize.width());
    const uint32_t swapchainHeight = uint32_t(m_swapchainSize.height());

    QVector<VkImage> swapchainImages;
    uint32_t numSwapchainImages = 0;
    vkGetSwapchainImagesKHR(*m_device, m_swapchain, &numSwapchainImages, nullptr);
    swapchainImages.resize(int(numSwapchainImages));
    if(VKFAILED(result = vkGetSwapchainImagesKHR(*m_device, m_swapchain, &numSwapchainImages, swapchainImages.data()))) {
        qCWarning(logVulkan) << "Failed to obtain swapchain image handles:" << result.toString();
        return;
    }

    m_swapchainAttachments.resize(int(numSwapchainImages));
    for(uint32_t imageIndex=0; imageIndex < numSwapchainImages; ++imageIndex) {
        auto &attachment = m_swapchainAttachments[int(imageIndex)];
        attachment.image.handle = swapchainImages[int(imageIndex)];
        attachment.image.view = m_device->createImageView({attachment.image, VK_IMAGE_VIEW_TYPE_2D, m_swapchainFormat.format});
        attachment.framebuffer = m_device->createFramebuffer({m_displayRenderPass, { attachment.image.view }, swapchainWidth, swapchainHeight});
    }

    constexpr VkFormat RenderBufferFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    for(auto &frame : m_frameResources) {
        ImageCreateInfo renderBufferCreateInfo{VK_IMAGE_TYPE_2D, RenderBufferFormat, m_swapchainSize};
        renderBufferCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if(!(frame.renderBuffer = m_device->createImage(renderBufferCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY))) {
            qCCritical(logVulkan) << "Failed to create render buffer";
            return;
        }

        const QVector<VkDescriptorSetLayout> descriptorSetLayouts = {
            m_displayPipeline.descriptorSetLayouts[0],
            m_testPipeline.descriptorSetLayouts[0],
        };
        const auto descriptorSets = m_device->allocateDescriptorSets({m_swapchainDescriptorPool, descriptorSetLayouts});
        frame.renderBufferSampleDS = descriptorSets[0];
        frame.renderBufferStorageDS = descriptorSets[1];

        const QVector<WriteDescriptorSet> descriptorWrites = {
            { frame.renderBufferSampleDS, 0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorImageInfo(frame.renderBuffer.view, ImageState::ShaderRead) },
            { frame.renderBufferStorageDS, 0, 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, DescriptorImageInfo(frame.renderBuffer.view, ImageState::ShaderReadWrite) },
        };
        m_device->writeDescriptorSets(descriptorWrites);
    }

    m_clearPreviousRenderBuffer = true;
}

void Renderer::releaseSwapchainResources()
{
    for(auto &attachment : m_swapchainAttachments) {
        m_device->destroyImageView(attachment.image.view);
        m_device->destroyFramebuffer(attachment.framebuffer);
    }
    m_swapchainAttachments.clear();

    for(auto &frame : m_frameResources) {
        m_device->destroyImage(frame.renderBuffer);
    }

    m_device->resetDescriptorPool(m_swapchainDescriptorPool);
    m_renderBuffersReady = false;
}

bool Renderer::querySwapchainProperties(VkPhysicalDevice physicalDevice, VkSurfaceFormatKHR &surfaceFormat, int &minImageCount) const
{
    VkSurfaceKHR surface = QVulkanInstance::surfaceForWindow(m_window);

    Result result;

    VkSurfaceCapabilitiesKHR surfaceCaps;
    if(VKFAILED(result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps))) {
        qCCritical(logVulkan) << "Failed to query physical device surface capabilities" << result.toString();
        return false;
    }

    QVector<VkSurfaceFormatKHR> surfaceFormats;
    uint32_t surfaceFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
    surfaceFormats.resize(int(surfaceFormatCount));
    if(VKFAILED(result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data()))) {
        qCCritical(logVulkan) << "Failed to enumerate physical device surface formats" << result.toString();
        return false;
    }

    surfaceFormat = surfaceFormats[0];
    minImageCount = int(surfaceCaps.minImageCount);
    return true;
}

void Renderer::resizeSwapchain()
{
    Q_ASSERT(m_device);
    Q_ASSERT(m_window);

    if(m_swapchainSize != m_window->size()) {
        m_device->waitIdle();
        if(m_swapchain) {
            releaseSwapchainResources();
        }
        Swapchain newSwapchain = m_device->createSwapchain(m_window, m_swapchainFormat, uint32_t(numConcurrentFrames()), m_swapchain);
        if(newSwapchain) {
            m_device->destroySwapchain(m_swapchain);
            m_swapchain = newSwapchain;
            m_swapchainSize = m_window->size();
            createSwapchainResources();
        }
        else {
            qCWarning(logVulkan) << "Failed to resize swapchain";
        }
    }
}

bool Renderer::acquireNextSwapchainImage(uint32_t &imageIndex) const
{
    Result result;
    if(VKFAILED(result = vkAcquireNextImageKHR(*m_device, m_swapchain, UINT64_MAX, m_presentationFinishedSemaphore, VK_NULL_HANDLE, &imageIndex))) {
        if(result != VK_SUBOPTIMAL_KHR) {
            qCCritical(logVulkan) << "Failed to acquire next swapchain image:" << result.toString();
            return false;
        }
    }
    return true;
}

bool Renderer::submitFrameCommandsAndPresent(uint32_t imageIndex)
{
    const FrameResources &frame = m_frameResources[m_frameIndex];

    Result result;

    VkPipelineStageFlags submitWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = m_presentationFinishedSemaphore;
    submitInfo.pWaitDstStageMask = &submitWaitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = frame.commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = m_renderingFinishedSemaphore;
    if(VKFAILED(result = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, frame.commandBuffersExecutedFence))) {
        qCCritical(logVulkan) << "Failed to submit frame commands to the graphics queue:" << result.toString();
        return false;
    }

    m_frameIndex = (m_frameIndex + 1) % int(numConcurrentFrames());

    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = m_renderingFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = m_swapchain;
    presentInfo.pImageIndices = &imageIndex;
    result = vkQueuePresentKHR(m_graphicsQueue, &presentInfo);
    if(VKSUCCEEDED(result) || result == VK_SUBOPTIMAL_KHR) {
        Q_ASSERT(m_instance);
        m_instance->presentQueued(m_window);
    }
    else if(result != VK_ERROR_OUT_OF_DATE_KHR) {
        qCCritical(logVulkan) << "Failed to queue swapchain image for presentation:" << result.toString();
        return false;
    }

    return true;
}

void Renderer::renderFrame()
{
    resizeSwapchain();
    const QRect renderRect = { {0, 0}, m_swapchainSize };

    FrameResources &currentFrame = m_frameResources[currentFrameIndex()];
    FrameResources &previousFrame = m_frameResources[previousFrameIndex()];

    m_device->waitForFence(currentFrame.commandBuffersExecutedFence);
    m_device->resetFence(currentFrame.commandBuffersExecutedFence);

    m_commandBufferManager->submitCommandBuffers(m_graphicsQueue);

    uint32_t swapchainImageIndex = 0;

    CommandBuffer &commandBuffer = currentFrame.commandBuffer;
    commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    {
        if(!m_renderBuffersReady) {
            QVector<ImageTransition> transitions(static_cast<int>(numConcurrentFrames()));
            for(int i=0; i<int(numConcurrentFrames()); ++i) {
                transitions[i] = { m_frameResources[i].renderBuffer, ImageState::Undefined, ImageState::ShaderReadWrite, VK_IMAGE_ASPECT_COLOR_BIT };
            }
            commandBuffer.resourceBarrier(transitions);
            m_renderBuffersReady = true;
        }
        if(m_clearPreviousRenderBuffer) {
            commandBuffer.clearColorImage(previousFrame.renderBuffer, ImageState::ShaderReadWrite);
            m_clearPreviousRenderBuffer = false;
        }

        const QVector<VkDescriptorSet> renderDescriptorSets = {
            currentFrame.renderBufferStorageDS,
            previousFrame.renderBufferStorageDS,
        };
        uint32_t dispatchGroupsX = uint32_t(qCeil(renderRect.width() / 8.0f));
        uint32_t dispatchGroupsY = uint32_t(qCeil(renderRect.height() / 8.0f));
        commandBuffer.bindPipeline(m_testPipeline);
        commandBuffer.bindDescriptorSets(m_testPipeline, 0, renderDescriptorSets);
        commandBuffer.dispatch(dispatchGroupsX, dispatchGroupsY);

        commandBuffer.resourceBarrier({currentFrame.renderBuffer, ImageState::ShaderReadWrite, ImageState::ShaderRead});

        if(acquireNextSwapchainImage(swapchainImageIndex)) {
            const auto &attachment = m_swapchainAttachments[int(swapchainImageIndex)];
            commandBuffer.beginRenderPass({m_displayRenderPass, attachment.framebuffer, renderRect}, VK_SUBPASS_CONTENTS_INLINE);
            commandBuffer.bindPipeline(m_displayPipeline);
            commandBuffer.bindDescriptorSets(m_displayPipeline, 0, {currentFrame.renderBufferSampleDS});
            commandBuffer.setViewport(renderRect);
            commandBuffer.setScissor(renderRect);
            commandBuffer.draw(3, 1);
            commandBuffer.endRenderPass();
        }

        commandBuffer.resourceBarrier({currentFrame.renderBuffer, ImageState::ShaderRead, ImageState::ShaderReadWrite});
    }
    commandBuffer.end();

    submitFrameCommandsAndPresent(swapchainImageIndex);

    m_commandBufferManager->proceedToNextFrame();
    m_frameAdvanceService->proceedToNextFrame();
}

VkPhysicalDevice Renderer::choosePhysicalDevice(const QByteArrayList &requiredExtensions, uint32_t &queueFamilyIndex) const
{
    Q_ASSERT(m_instance);

    VkPhysicalDevice selectedPhysicalDevice = VK_NULL_HANDLE;
    uint32_t selectedQueueFamilyIndex = uint32_t(-1);

    QVector<VkPhysicalDevice> physicalDevices;
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance->vkInstance(), &physicalDeviceCount, nullptr);
    if(physicalDeviceCount > 0) {
        physicalDevices.resize(int(physicalDeviceCount));
        if(VKFAILED(vkEnumeratePhysicalDevices(m_instance->vkInstance(), &physicalDeviceCount, physicalDevices.data()))) {
            qCWarning(logVulkan) << "Failed to enumerate available physical devices";
            return VK_NULL_HANDLE;
        }
    }
    else {
        qCWarning(logVulkan) << "No Vulkan capable physical devices found";
        return VK_NULL_HANDLE;
    }

    for(VkPhysicalDevice physicalDevice : physicalDevices) {
        QVector<VkQueueFamilyProperties> queueFamilies;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        if(queueFamilyCount == 0) {
            continue;
        }
        queueFamilies.resize(int(queueFamilyCount));
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        selectedQueueFamilyIndex = uint32_t(-1);
        for(uint32_t index=0; index < queueFamilyCount; ++index) {
            const auto &queueFamily = queueFamilies[int(index)];

            constexpr VkQueueFlags RequiredQueueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
            if((queueFamily.queueFlags & RequiredQueueFlags) != RequiredQueueFlags) {
                continue;
            }
            if(!m_instance->supportsPresent(physicalDevice, index, m_window)) {
                continue;
            }
            selectedQueueFamilyIndex = index;
            break;
        }
        if(selectedQueueFamilyIndex == uint32_t(-1)) {
            continue;
        }

        QVector<VkExtensionProperties> extensions;
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        if(extensionCount == 0) {
            continue;
        }
        extensions.resize(int(extensionCount));
        if(VKFAILED(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data()))) {
            qCWarning(logVulkan) << "Failed to enumerate device extensions for physical device:" << physicalDevice;
            continue;
        }

        bool allRequiredExtensionsFound = true;
        for(const QByteArray &requiredExtension : requiredExtensions) {
            bool extensionFound = false;
            for(const VkExtensionProperties &extension : extensions) {
                if(requiredExtension == extension.extensionName) {
                    extensionFound = true;
                    break;
                }
            }
            if(!extensionFound) {
                allRequiredExtensionsFound = false;
                break;
            }
        }
        if(!allRequiredExtensionsFound) {
            continue;
        }

        selectedPhysicalDevice = physicalDevice;
        break;
    }
    Q_ASSERT(selectedPhysicalDevice != VK_NULL_HANDLE);

    VkPhysicalDeviceProperties selectedDeviceProperties;
    vkGetPhysicalDeviceProperties(selectedPhysicalDevice, &selectedDeviceProperties);
    qCInfo(logVulkan) << "Selected physical device:" << selectedDeviceProperties.deviceName;

    queueFamilyIndex = selectedQueueFamilyIndex;
    return selectedPhysicalDevice;
}

RenderPass Renderer::createDisplayRenderPass(VkFormat swapchainFormat) const
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapchainFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    RenderPassCreateInfo createInfo;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorAttachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;

    RenderPass renderPass;
    if(!(renderPass = m_device->createRenderPass(createInfo))) {
        qCCritical(logVulkan) << "Could not create display render pass";
        return VK_NULL_HANDLE;
    }
    return renderPass;
}

int Renderer::currentFrameIndex() const
{
    return m_frameIndex;
}

int Renderer::previousFrameIndex() const
{
    return (m_frameIndex > 0) ? (m_frameIndex - 1) : (int(numConcurrentFrames()) - 1);
}

QSurface *Renderer::surface() const
{
    return m_window;
}

void Renderer::setSurface(QObject *surfaceObject)
{
    if(surfaceObject) {
        if(QWindow *window = qobject_cast<QWindow*>(surfaceObject)) {
            m_window = window;
        }
        else {
            qCWarning(logVulkan) << "Incompatible surface object: expected QWindow instance";
        }
    }
    else {
        m_window = nullptr;
    }
}

Device *Renderer::device() const
{
    Q_ASSERT(m_device);
    return m_device.get();
}

void Renderer::markDirty(DirtySet changes, Raytrace::BackendNode *node)
{
    Q_UNUSED(node);
    m_dirtySet |= changes;
}

Raytrace::Entity *Renderer::sceneRoot() const
{
    return m_sceneRoot;
}

void Renderer::setSceneRoot(Raytrace::Entity *rootEntity)
{
    m_sceneRoot = rootEntity;
    m_updateWorldTransformJob->setRoot(m_sceneRoot);
}

void Renderer::setNodeManagers(Raytrace::NodeManagers *nodeManagers)
{
    m_nodeManagers = nodeManagers;
}

Qt3DCore::QAbstractFrameAdvanceService *Renderer::frameAdvanceService() const
{
    return m_frameAdvanceService.get();
}

CommandBufferManager *Renderer::commandBufferManager() const
{
    return m_commandBufferManager.get();
}

QVector<Qt3DCore::QAspectJobPtr> Renderer::renderJobs()
{
    QVector<Qt3DCore::QAspectJobPtr> jobs;

    if(m_dirtySet & DirtyFlag::TransformDirty) {
        jobs.append(m_updateWorldTransformJob);
    }
    if(m_dirtySet & DirtyFlag::GeometryDirty) {

    }

    jobs.append(m_destroyExpiredResourcesJob);

    m_dirtySet = DirtyFlag::NoneDirty;
    return jobs;
}

uint32_t Renderer::numConcurrentFrames() const
{
    return uint32_t(m_frameResources.size());
}

} // Vulkan
} // Qt3DRaytrace
