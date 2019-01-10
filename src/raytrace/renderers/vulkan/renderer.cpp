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

#include <QVulkanWindow>
#include <QtMath>

namespace Qt3DRaytrace {
namespace Vulkan {

Q_LOGGING_CATEGORY(logVulkan, "raytrace.vulkan")

Renderer::Renderer()
    : m_frameAdvanceService(new FrameAdvanceService)
{}

void Renderer::preInitResources()
{
    static const QByteArrayList RequiredDeviceExtensions {
        VK_NV_RAY_TRACING_EXTENSION_NAME,
    };

    if(VKFAILED(volkInitialize())) {
        qCCritical(logVulkan) << "Failed to initialize Vulkan function loader";
        return;
    }

    QVulkanInstance *instance = m_window->vulkanInstance();
    Q_ASSERT(instance);
    volkLoadInstance(instance->vkInstance());

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    for(int physicalDeviceIndex=0; physicalDeviceIndex < m_window->availablePhysicalDevices().size(); ++physicalDeviceIndex) {
        m_window->setPhysicalDeviceIndex(physicalDeviceIndex);
        auto deviceExtensions = m_window->supportedDeviceExtensions();

        bool requiredDeviceExtensionsSupported = true;
        for(const auto& requiredExtension : RequiredDeviceExtensions) {
            if(!deviceExtensions.contains(requiredExtension)) {
                requiredDeviceExtensionsSupported = false;
                break;
            }
        }
        if(requiredDeviceExtensionsSupported) {
            physicalDevice = m_window->physicalDevice();
            break;
        }
    }
    if(physicalDevice) {
        qCInfo(logVulkan) << "Using physical device:" << m_window->physicalDeviceProperties()->deviceName;
        m_window->setDeviceExtensions(RequiredDeviceExtensions);
    }
    else {
        qCCritical(logVulkan) << "No suitable physical device found";
    }
}

void Renderer::initResources()
{
    m_device.reset(new Vulkan::Device(m_window->device(), m_window->physicalDevice()));
    if(!m_device->isValid()) {
        qCCritical(logVulkan) << "Failed to initialize device object";
        return;
    }

    const uint32_t numFrames = uint32_t(m_window->concurrentFrameCount());
    m_frameResources.resize(int(numFrames));

    {
        const QVector<VkDescriptorPoolSize> descriptorPoolSizes{
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, numFrames },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, numFrames },
        };
        m_swapChainDescriptorPool = m_device->createDescriptorPool({2 * numFrames, descriptorPoolSizes});
    }

    m_queryPool = m_device->createQueryPool({VK_QUERY_TYPE_TIMESTAMP, 2 * numFrames});

    m_defaultSampler = m_device->createSampler({VK_FILTER_NEAREST});

    m_displayPipeline = Vulkan::GraphicsPipelineBuilder(m_device.get(), m_window->defaultRenderPass())
            .shaders({"display.vert", "display.frag"})
            .defaultSampler(m_defaultSampler)
            .build();
    if(!m_displayPipeline) {
        qCCritical(logVulkan) << "Failed to create display graphics pipeline";
        return;
    }

    m_testPipeline = Vulkan::ComputePipelineBuilder(m_device.get())
            .shaders({"test.comp"})
            .build();
}

void Renderer::initSwapChainResources()
{
    const VkExtent3D swapChainExtent = {
        uint32_t(m_window->swapChainImageSize().width()),
        uint32_t(m_window->swapChainImageSize().height()),
        1,
    };

    for(auto &frame : m_frameResources) {
        Vulkan::ImageCreateInfo renderBufferCreateInfo{VK_IMAGE_TYPE_2D, VK_FORMAT_R16G16B16A16_SFLOAT, swapChainExtent};
        renderBufferCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if(!(frame.renderBuffer = m_device->createImage(renderBufferCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY))) {
            qCCritical(logVulkan) << "Failed to create render buffer image";
            return;
        }

        const QVector<VkDescriptorSetLayout> descriptorSetLayouts{
            m_displayPipeline.descriptorSetLayouts.at(0),
                    m_testPipeline.descriptorSetLayouts.at(0),
        };
        const auto descriptorSets = m_device->allocateDescriptorSets({m_swapChainDescriptorPool, descriptorSetLayouts});
        frame.renderBufferSampleDS = descriptorSets.at(0);
        frame.renderBufferStorageDS = descriptorSets.at(1);

        QVector<Vulkan::WriteDescriptorSet> descriptorWrites = {
            { frame.renderBufferSampleDS, 0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Vulkan::DescriptorImageInfo(frame.renderBuffer.view, Vulkan::ImageState::ShaderRead) },
            { frame.renderBufferStorageDS, 0, 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, Vulkan::DescriptorImageInfo(frame.renderBuffer.view, Vulkan::ImageState::ShaderReadWrite) },
        };
        m_device->writeDescriptorSets(descriptorWrites);
    }

    m_clearPreviousRenderBuffer = true;
}

void Renderer::releaseSwapChainResources()
{
    for(auto &frame : m_frameResources) {
        m_device->destroyImage(frame.renderBuffer);
    }
    m_device->resetDescriptorPool(m_swapChainDescriptorPool);
    m_renderBuffersReady = false;
}

void Renderer::releaseResources()
{
    m_device->destroyPipeline(m_testPipeline);
    m_device->destroyPipeline(m_displayPipeline);

    m_device->destroySampler(m_defaultSampler);
    m_device->destroyQueryPool(m_queryPool);
    m_device->destroyDescriptorPool(m_swapChainDescriptorPool);

    m_device.reset();
}

void Renderer::startNextFrame()
{
    const VkRect2D renderArea = {
        { 0, 0 },
        { uint32_t(m_window->width()), uint32_t(m_window->height()) },
    };
    const VkViewport renderViewport = {
        0.0f, 0.0f, float(m_window->width()), float(m_window->height()),
    };

    const int currentFrameIndex = m_window->currentFrame();
    const int prevFrameIndex = (currentFrameIndex > 0) ? (currentFrameIndex - 1) : (m_window->concurrentFrameCount() - 1);
    auto &frame = m_frameResources[currentFrameIndex];
    auto &prevFrame = m_frameResources[prevFrameIndex];

    Vulkan::CommandBuffer commandBuffer(m_window->currentCommandBuffer());
    VkFramebuffer framebuffer = m_window->currentFramebuffer();

    if(!m_renderBuffersReady) {
        QVector<Vulkan::ImageTransition> transitions(m_frameResources.size());
        for(int i=0; i<transitions.size(); ++i) {
            transitions[i] = { m_frameResources[i].renderBuffer.handle, Vulkan::ImageState::Undefined, Vulkan::ImageState::ShaderReadWrite, VK_IMAGE_ASPECT_COLOR_BIT };
        }
        commandBuffer.resourceBarrier(transitions);
        m_renderBuffersReady = true;
    }
    if(m_clearPreviousRenderBuffer) {
        commandBuffer.clearColorImage(prevFrame.renderBuffer, Vulkan::ImageState::ShaderReadWrite);
        m_clearPreviousRenderBuffer = false;
    }

    const VkDescriptorSet renderDescriptorSets[] = {
        frame.renderBufferStorageDS,
        prevFrame.renderBufferStorageDS,
    };
    uint32_t dispatchCountX = uint32_t(qCeil(renderViewport.width / 8.0f));
    uint32_t dispatchCountY = uint32_t(qCeil(renderViewport.height / 8.0f));
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_testPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_testPipeline.pipelineLayout, 0, 2, renderDescriptorSets, 0, nullptr);
    vkCmdDispatch(commandBuffer, dispatchCountX, dispatchCountY, 1);

    commandBuffer.resourceBarrier({frame.renderBuffer, Vulkan::ImageState::ShaderReadWrite, Vulkan::ImageState::ShaderRead});

    const VkClearValue renderPassClearValues[2] = {{}, {}};
    Vulkan::RenderPassBeginInfo renderPassBeginInfo(m_window->defaultRenderPass(), framebuffer);
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = renderPassClearValues;
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    const VkDescriptorSet displayDescriptorSets[] = {
        frame.renderBufferSampleDS,
    };
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_displayPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_displayPipeline.pipelineLayout, 0, 1, displayDescriptorSets, 0, nullptr);
    vkCmdSetViewport(commandBuffer, 0, 1, &renderViewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &renderArea);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    commandBuffer.resourceBarrier({frame.renderBuffer, Vulkan::ImageState::ShaderRead, Vulkan::ImageState::ShaderReadWrite});

    m_window->frameReady();
    m_frameAdvanceService->proceedToNextFrame();

    m_window->requestUpdate();
}

QSurface *Renderer::surface() const
{
    return m_window;
}

void Renderer::setSurface(QObject *surfaceObject)
{
    if(surfaceObject) {
        if(QVulkanWindow *window = qobject_cast<QVulkanWindow*>(surfaceObject)) {
            m_window = window;
        }
        else {
            qCWarning(logVulkan) << "Incompatible surface object: expected QVulkanWindow instance";
        }
    }
    else {
        m_window = nullptr;
    }
}

Qt3DCore::QAbstractFrameAdvanceService *Renderer::frameAdvanceService() const
{
    return m_frameAdvanceService.get();
}

} // Vulkan
} // Qt3DRaytrace
