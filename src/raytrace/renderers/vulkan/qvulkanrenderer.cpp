/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/qvulkanrenderer_p.h>

#include <QVulkanWindow>

namespace Qt3DRaytrace {

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

}

void QVulkanRenderer::initResources()
{

}

void QVulkanRenderer::initSwapChainResources()
{

}

void QVulkanRenderer::releaseSwapChainResources()
{

}

void QVulkanRenderer::releaseResources()
{

}

void QVulkanRenderer::startNextFrame()
{
    Q_D(QVulkanRenderer);



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
