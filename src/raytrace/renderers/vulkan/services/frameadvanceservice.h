/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>
#include <Qt3DCore/private/qabstractframeadvanceservice_p.h>

#include <QSemaphore>
#include <QElapsedTimer>

namespace Qt3DRaytrace {
namespace Vulkan {

class FrameAdvanceService final : public Qt3DCore::QAbstractFrameAdvanceService
{
public:
    FrameAdvanceService();

    qint64 waitForNextFrame() override;
    void start() override;
    void stop() override;

    void proceedToNextFrame();

private:
    QSemaphore m_semaphore;
    QElapsedTimer m_elapsedTimer;
};

} // Vulkan
} // Qt3DRaytrace
