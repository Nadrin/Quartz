/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qvulkanrenderer.h>
#include <renderers/vulkan/vulkanframeadvanceservice.h>

#include <vk_mem_alloc.h>

namespace Qt3DRaytrace {

class QVulkanRendererPrivate
{
public:
    explicit QVulkanRendererPrivate(QVulkanRenderer *q, QVulkanWindow *window);

    QVulkanWindow *m_window;
    QScopedPointer<VulkanFrameAdvanceService> m_frameAdvanceService;

    VkDevice m_device;
    VmaAllocator m_allocator;

    Q_DECLARE_PUBLIC(QVulkanRenderer)

private:
    QVulkanRenderer *q_ptr;
};

} // Qt3DRaytrace
