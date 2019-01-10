/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytraceExtras/qt3draytraceextras_global.h>
#include <Qt3DRaytrace/qrendererinterface.h>

#include <QVulkanWindowRenderer>

namespace Qt3DRaytraceExtras {

class Qt3DWindowRenderer final : public QVulkanWindowRenderer
{
public:
    explicit Qt3DWindowRenderer(QVulkanWindow *window);

    Qt3DRaytrace::QRendererInterface *rendererInterface() const
    {
        return m_renderer;
    }
    void setRendererInterface(Qt3DRaytrace::QRendererInterface *rendererInterface);

    void preInitResources() override;
    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;
    void startNextFrame() override;

private:
    QVulkanWindow *m_window;
    Qt3DRaytrace::QRendererInterface *m_renderer;
};

} // Qt3DRaytraceExtras
