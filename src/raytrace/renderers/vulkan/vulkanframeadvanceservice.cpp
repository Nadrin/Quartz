/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/vulkanframeadvanceservice.h>

namespace Qt3DRaytrace {

VulkanFrameAdvanceService::VulkanFrameAdvanceService()
    : Qt3DCore::QAbstractFrameAdvanceService(QStringLiteral("Vulkan Frame Advance Service"))
{}

qint64 VulkanFrameAdvanceService::waitForNextFrame()
{
    m_semaphore.acquire();
    return m_elapsedTimer.nsecsElapsed();
}

void VulkanFrameAdvanceService::start()
{
    m_elapsedTimer.start();
}

void VulkanFrameAdvanceService::stop()
{
    proceedToNextFrame();
}

void VulkanFrameAdvanceService::proceedToNextFrame()
{
    m_semaphore.release();
}

} // Qt3DRaytrace
