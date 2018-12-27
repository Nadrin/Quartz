/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>
#include <Qt3DRaytrace/qabstractrenderer.h>

#include <QVulkanWindowRenderer>
#include <QScopedPointer>

namespace Qt3DRaytrace {

class QVulkanRendererPrivate;

class QT3DRAYTRACESHARED_EXPORT QVulkanRenderer final : public QAbstractRenderer
                                                      , public QVulkanWindowRenderer
{
public:
    QVulkanRenderer(QVulkanWindow *window);

    void preInitResources() override;
    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;
    void startNextFrame() override;

    API api() const override { return QAbstractRenderer::API::Vulkan; }
    QSurface *surface() const override;
    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const override;

    Q_DECLARE_PRIVATE(QVulkanRenderer)

private:
    bool initVulkanAllocator();
    void releaseVulkanAllocator();

    QScopedPointer<QVulkanRendererPrivate> d_ptr;
};

} // Qt3DRaytrace
