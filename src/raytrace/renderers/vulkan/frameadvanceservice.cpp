/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/frameadvanceservice.h>

namespace Qt3DRaytrace {
namespace Vulkan {

FrameAdvanceService::FrameAdvanceService()
    : Qt3DCore::QAbstractFrameAdvanceService(QStringLiteral("Vulkan Frame Advance Service"))
{}

qint64 FrameAdvanceService::waitForNextFrame()
{
    m_semaphore.acquire();
    return m_elapsedTimer.nsecsElapsed();
}

void FrameAdvanceService::start()
{
    m_elapsedTimer.start();
}

void FrameAdvanceService::stop()
{
    proceedToNextFrame();
}

void FrameAdvanceService::proceedToNextFrame()
{
    m_semaphore.release();
}

} // Vulkan
} // Qt3DRaytrace
