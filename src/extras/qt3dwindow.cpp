/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <Qt3DRaytraceExtras/qt3dwindow.h>
#include <Qt3DRaytrace/qvulkanrenderer.h>

#include <QEntity>

#if QT_CONFIG(vulkan)
#endif

namespace Qt3DRaytraceExtras {

Qt3DWindow::Qt3DWindow()
    : m_aspectEngine(new Qt3DCore::QAspectEngine)
    , m_raytraceAspect(new Qt3DRaytrace::QRaytraceAspect)
    , m_inputAspect(new Qt3DInput::QInputAspect)
    , m_logicAspect(new Qt3DLogic::QLogicAspect)
    , m_inputSettings(new Qt3DInput::QInputSettings)
    , m_root(new Qt3DCore::QEntity)
    , m_userRoot(nullptr)
{
    resize(1024, 768);
    setFlags(QVulkanWindow::PersistentResources);

    m_aspectEngine->registerAspect(m_inputAspect);
    m_aspectEngine->registerAspect(m_logicAspect);

    m_inputSettings->setEventSource(this);
}

QVulkanWindowRenderer *Qt3DWindow::createRenderer()
{
    auto renderer = new Qt3DRaytrace::QVulkanRenderer(this);
    m_raytraceAspect->setRenderer(renderer);
    m_aspectEngine->registerAspect(m_raytraceAspect);
    return renderer;
}

void Qt3DWindow::setRootEntity(Qt3DCore::QEntity *root)
{
    if(m_userRoot != root) {
        if(m_userRoot) {
            m_userRoot->setParent(static_cast<Qt3DCore::QNode*>(nullptr));
        }
        if(root) {
            root->setParent(m_root);
        }
        m_userRoot = root;
    }
}

void Qt3DWindow::exposeEvent(QExposeEvent *event)
{
    QVulkanWindow::exposeEvent(event);

    if(!m_initialized) {
        m_root->addComponent(m_inputSettings.get());
        m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr(m_root));
        m_initialized = true;
    }
}

} // Qt3DRaytraceExtras
