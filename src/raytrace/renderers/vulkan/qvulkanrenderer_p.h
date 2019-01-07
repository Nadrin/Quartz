/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qvulkanrenderer.h>
#include <renderers/vulkan/vulkanframeadvanceservice.h>

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/initializers.h>
#include <renderers/vulkan/device.h>

#include <QVector>

namespace Qt3DRaytrace {

class QVulkanRendererPrivate
{
public:
    explicit QVulkanRendererPrivate(QVulkanRenderer *q, QVulkanWindow *window);

    QVulkanWindow *m_window;
    QScopedPointer<VulkanFrameAdvanceService> m_frameAdvanceService;
    QScopedPointer<Vulkan::Device> m_device;

    struct FrameResources {
        Vulkan::Image renderBuffer;
        Vulkan::DescriptorSet renderBufferSampleDS;
        Vulkan::DescriptorSet renderBufferStorageDS;
    };
    QVector<FrameResources> m_frameResources;
    bool m_renderBuffersReady = false;
    bool m_clearPreviousRenderBuffer = false;

    Vulkan::DescriptorPool m_defaultDescriptorPool;
    Vulkan::DescriptorPool m_swapChainDescriptorPool;
    Vulkan::QueryPool m_queryPool;
    Vulkan::Sampler m_defaultSampler;

    Vulkan::Pipeline m_displayPipeline;
    Vulkan::Pipeline m_testPipeline;

    Q_DECLARE_PUBLIC(QVulkanRenderer)

private:
    QVulkanRenderer *q_ptr;
};

} // Qt3DRaytrace
