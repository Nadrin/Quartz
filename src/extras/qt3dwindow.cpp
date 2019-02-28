/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <qt3dwindow_p.h>

#include <Qt3DCore/QEntity>
#include <QPlatformSurfaceEvent>

namespace Qt3DRaytraceExtras {

Qt3DWindowPrivate::Qt3DWindowPrivate()
    : m_aspectEngine(new Qt3DCore::QAspectEngine)
    , m_raytraceAspect(new Qt3DRaytrace::QRaytraceAspect)
    , m_inputAspect(new Qt3DInput::QInputAspect)
    , m_logicAspect(new Qt3DLogic::QLogicAspect)
    , m_inputSettings(new Qt3DInput::QInputSettings)
    , m_root(new Qt3DCore::QEntity)
    , m_userRoot(nullptr)
{}

Qt3DWindowPrivate::~Qt3DWindowPrivate()
{
    if(!m_initialized) {
        delete m_inputSettings;
    }
}

Qt3DWindow::Qt3DWindow(QWindow *parent)
    : QWindow(*new Qt3DWindowPrivate, parent)
{
    Q_D(Qt3DWindow);

    resize(1024, 768);
    setSurfaceType(SurfaceType::VulkanSurface);
    setTitle("Quartz Viewport");

    QObject::connect(this, &Qt3DWindow::aboutToClose, d->m_raytraceAspect, &Qt3DRaytrace::QRaytraceAspect::suspendJobs);

    d->m_aspectEngine->registerAspect(d->m_raytraceAspect);
    d->m_aspectEngine->registerAspect(d->m_inputAspect);
    d->m_aspectEngine->registerAspect(d->m_logicAspect);

    d->m_inputSettings->setEventSource(this);
    d->m_raytraceAspect->setSurface(this);
}

void Qt3DWindow::registerAspect(Qt3DCore::QAbstractAspect *aspect)
{
    Q_ASSERT(!isVisible());
    Q_D(Qt3DWindow);
    d->m_aspectEngine->registerAspect(aspect);
}

void Qt3DWindow::registerAspect(const QString &name)
{
    Q_ASSERT(!isVisible());
    Q_D(Qt3DWindow);
    d->m_aspectEngine->registerAspect(name);
}

void Qt3DWindow::setRootEntity(Qt3DCore::QEntity *root)
{
    Q_D(Qt3DWindow);

    if(d->m_userRoot != root) {
        if(d->m_userRoot) {
            d->m_userRoot->setParent(static_cast<Qt3DCore::QNode*>(nullptr));
        }
        if(root) {
            root->setParent(d->m_root);
        }
        d->m_userRoot = root;
    }
}

bool Qt3DWindow::event(QEvent *event)
{
    Q_D(Qt3DWindow);

    switch(event->type()) {
    case QEvent::Close:
        emit aboutToClose();
        break;
    case QEvent::PlatformSurface:
        if(static_cast<QPlatformSurfaceEvent*>(event)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
            d->m_raytraceAspect->setSurface(nullptr);
        }
        break;
    default:
        break;
    }

    return QWindow::event(event);
}

void Qt3DWindow::showEvent(QShowEvent *event)
{
    Q_D(Qt3DWindow);

    QWindow::showEvent(event);

    if(!d->m_initialized) {
        d->m_root->addComponent(d->m_inputSettings);
        d->m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr(d->m_root));
        d->m_initialized = true;
    }
}

} // Qt3DRaytraceExtras
