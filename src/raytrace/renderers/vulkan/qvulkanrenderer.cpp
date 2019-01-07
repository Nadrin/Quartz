/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/qvulkanrenderer_p.h>
#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/commandbuffer.h>
#include <renderers/vulkan/initializers.h>
#include <renderers/vulkan/shadermodule.h>
#include <renderers/vulkan/pipeline/graphicspipeline.h>
#include <renderers/vulkan/pipeline/computepipeline.h>

#include <QVulkanWindow>
#include <QtMath>

namespace Qt3DRaytrace {

Q_LOGGING_CATEGORY(logVulkan, "raytrace.vulkan")

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
        qCCritical(logVulkan) << "Failed to initialize Vulkan function loader";
        return;
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
        qCInfo(logVulkan) << "Using physical device:" << d->m_window->physicalDeviceProperties()->deviceName;
        d->m_window->setDeviceExtensions(RequiredDeviceExtensions);
    }
    else {
        qCCritical(logVulkan) << "No suitable physical device found";
    }
}

void QVulkanRenderer::initResources()
{
    Q_D(QVulkanRenderer);

    d->m_device.reset(new Vulkan::Device(d->m_window->device(), d->m_window->physicalDevice()));
    if(!d->m_device->isValid()) {
        qCCritical(logVulkan) << "Failed to initialize device object";
        return;
    }

    const uint32_t numFrames = uint32_t(d->m_window->concurrentFrameCount());
    d->m_frameResources.resize(int(numFrames));

    {
        const QVector<VkDescriptorPoolSize> descriptorPoolSizes{
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, numFrames },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, numFrames },
        };
        d->m_swapChainDescriptorPool = d->m_device->createDescriptorPool({2 * numFrames, descriptorPoolSizes});
    }

    d->m_queryPool = d->m_device->createQueryPool({VK_QUERY_TYPE_TIMESTAMP, 2 * numFrames});

    d->m_defaultSampler = d->m_device->createSampler({VK_FILTER_NEAREST});

    d->m_displayPipeline = Vulkan::GraphicsPipelineBuilder(d->m_device.get(), d->m_window->defaultRenderPass())
            .shaders({"display.vert", "display.frag"})
            .defaultSampler(d->m_defaultSampler)
            .build();
    if(!d->m_displayPipeline) {
        qCCritical(logVulkan) << "Failed to create display graphics pipeline";
        return;
    }

    d->m_testPipeline = Vulkan::ComputePipelineBuilder(d->m_device.get())
            .shaders({"test.comp"})
            .build();
}

void QVulkanRenderer::initSwapChainResources()
{
    Q_D(QVulkanRenderer);

    const VkExtent3D swapChainExtent = {
        uint32_t(d->m_window->swapChainImageSize().width()),
        uint32_t(d->m_window->swapChainImageSize().height()),
        1,
    };

    for(auto &frame : d->m_frameResources) {
        Vulkan::ImageCreateInfo renderBufferCreateInfo{VK_IMAGE_TYPE_2D, VK_FORMAT_R16G16B16A16_SFLOAT, swapChainExtent};
        renderBufferCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if(!(frame.renderBuffer = d->m_device->createImage(renderBufferCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY))) {
            qCCritical(logVulkan) << "Failed to create render buffer image";
            return;
        }

        const QVector<VkDescriptorSetLayout> descriptorSetLayouts{
            d->m_displayPipeline.descriptorSetLayouts.at(0),
                    d->m_testPipeline.descriptorSetLayouts.at(0),
        };
        const auto descriptorSets = d->m_device->allocateDescriptorSets({d->m_swapChainDescriptorPool, descriptorSetLayouts});
        frame.renderBufferSampleDS = descriptorSets.at(0);
        frame.renderBufferStorageDS = descriptorSets.at(1);

        QVector<Vulkan::WriteDescriptorSet> descriptorWrites = {
            { frame.renderBufferSampleDS, 0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Vulkan::DescriptorImageInfo(frame.renderBuffer.view, Vulkan::ImageState::ShaderRead) },
            { frame.renderBufferStorageDS, 0, 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, Vulkan::DescriptorImageInfo(frame.renderBuffer.view, Vulkan::ImageState::ShaderReadWrite) },
        };
        d->m_device->writeDescriptorSets(descriptorWrites);
    }

    d->m_clearPreviousRenderBuffer = true;
}

void QVulkanRenderer::releaseSwapChainResources()
{
    Q_D(QVulkanRenderer);
    for(auto &frame : d->m_frameResources) {
        d->m_device->destroyImage(frame.renderBuffer);
    }
    d->m_device->resetDescriptorPool(d->m_swapChainDescriptorPool);
    d->m_renderBuffersReady = false;
}

void QVulkanRenderer::releaseResources()
{
    Q_D(QVulkanRenderer);

    d->m_device->destroyPipeline(d->m_testPipeline);
    d->m_device->destroyPipeline(d->m_displayPipeline);

    d->m_device->destroySampler(d->m_defaultSampler);
    d->m_device->destroyQueryPool(d->m_queryPool);
    d->m_device->destroyDescriptorPool(d->m_swapChainDescriptorPool);

    d->m_device.reset();
}

void QVulkanRenderer::startNextFrame()
{
    Q_D(QVulkanRenderer);

    const VkRect2D renderArea = {
        { 0, 0 },
        { uint32_t(d->m_window->width()), uint32_t(d->m_window->height()) },
    };
    const VkViewport renderViewport = {
        0.0f, 0.0f, float(d->m_window->width()), float(d->m_window->height()),
    };

    const int currentFrameIndex = d->m_window->currentFrame();
    const int prevFrameIndex = (currentFrameIndex > 0) ? (currentFrameIndex - 1) : (d->m_window->concurrentFrameCount() - 1);
    auto &frame = d->m_frameResources[currentFrameIndex];
    auto &prevFrame = d->m_frameResources[prevFrameIndex];

    Vulkan::CommandBuffer commandBuffer(d->m_window->currentCommandBuffer());
    VkFramebuffer framebuffer = d->m_window->currentFramebuffer();

    if(!d->m_renderBuffersReady) {
        QVector<Vulkan::ImageTransition> transitions(d->m_frameResources.size());
        for(int i=0; i<transitions.size(); ++i) {
            transitions[i] = { d->m_frameResources[i].renderBuffer.handle, Vulkan::ImageState::Undefined, Vulkan::ImageState::ShaderReadWrite, VK_IMAGE_ASPECT_COLOR_BIT };
        }
        commandBuffer.resourceBarrier(transitions);
        d->m_renderBuffersReady = true;
    }
    if(d->m_clearPreviousRenderBuffer) {
        commandBuffer.clearColorImage(prevFrame.renderBuffer, Vulkan::ImageState::ShaderReadWrite);
        d->m_clearPreviousRenderBuffer = false;
    }

    const VkDescriptorSet renderDescriptorSets[] = {
        frame.renderBufferStorageDS,
        prevFrame.renderBufferStorageDS,
    };
    uint32_t dispatchCountX = uint32_t(qCeil(renderViewport.width / 8.0f));
    uint32_t dispatchCountY = uint32_t(qCeil(renderViewport.height / 8.0f));
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, d->m_testPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, d->m_testPipeline.pipelineLayout, 0, 2, renderDescriptorSets, 0, nullptr);
    vkCmdDispatch(commandBuffer, dispatchCountX, dispatchCountY, 1);

    commandBuffer.resourceBarrier({frame.renderBuffer, Vulkan::ImageState::ShaderReadWrite, Vulkan::ImageState::ShaderRead});

    const VkClearValue renderPassClearValues[2] = {{}, {}};
    Vulkan::RenderPassBeginInfo renderPassBeginInfo(d->m_window->defaultRenderPass(), framebuffer);
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = renderPassClearValues;
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    const VkDescriptorSet displayDescriptorSets[] = {
        frame.renderBufferSampleDS,
    };
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, d->m_displayPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, d->m_displayPipeline.pipelineLayout, 0, 1, displayDescriptorSets, 0, nullptr);
    vkCmdSetViewport(commandBuffer, 0, 1, &renderViewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &renderArea);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    commandBuffer.resourceBarrier({frame.renderBuffer, Vulkan::ImageState::ShaderRead, Vulkan::ImageState::ShaderReadWrite});

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

} // Qt3DRaytrace
