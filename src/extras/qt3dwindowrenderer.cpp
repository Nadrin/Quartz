/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <qt3dwindowrenderer_p.h>

namespace Qt3DRaytraceExtras {

Qt3DWindowRenderer::Qt3DWindowRenderer(QVulkanWindow *window)
    : m_window(window)
    , m_renderer(nullptr)
{
    Q_ASSERT(m_window);
}

void Qt3DWindowRenderer::setRendererInterface(Qt3DRaytrace::QRendererInterface *rendererInterface)
{
    m_renderer = rendererInterface;
    if(m_renderer) {
        Q_ASSERT(m_renderer->api() == Qt3DRaytrace::QRendererInterface::API::Vulkan);
        m_renderer->setSurface(m_window);
    }
}

void Qt3DWindowRenderer::preInitResources()
{
    if(m_renderer) {
        m_renderer->preInitResources();
    }
}

void Qt3DWindowRenderer::initResources()
{
    if(m_renderer) {
        m_renderer->initResources();
    }
}

void Qt3DWindowRenderer::initSwapChainResources()
{
    if(m_renderer) {
        m_renderer->initSwapChainResources();
    }
}

void Qt3DWindowRenderer::releaseSwapChainResources()
{
    if(m_renderer) {
        m_renderer->releaseSwapChainResources();
    }
}

void Qt3DWindowRenderer::releaseResources()
{
    if(m_renderer) {
        m_renderer->releaseResources();
    }
}

void Qt3DWindowRenderer::startNextFrame()
{
    if(m_renderer) {
        m_renderer->startNextFrame();
    }
}

} // Qt3DRaytraceExtras
