/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <qt3dquickwindow_p.h>

#include <Qt3DCore/QEntity>
#include <Qt3DRaytrace/qrendererinterface.h>

#include <QPlatformSurfaceEvent>

namespace Qt3DRaytraceExtras {
namespace Quick {

Qt3DQuickWindowPrivate::Qt3DQuickWindowPrivate()
    : m_engine(new Qt3DCore::Quick::QQmlAspectEngine)
    , m_raytraceAspect(new Qt3DRaytrace::QRaytraceAspect)
    , m_inputAspect(new Qt3DInput::QInputAspect)
    , m_logicAspect(new Qt3DLogic::QLogicAspect)
    , m_cameraAspectRatioMode(Qt3DQuickWindow::AutomaticAspectRatio)
    , m_initialized(false)
{}

Qt3DQuickWindow::Qt3DQuickWindow(QWindow *parent)
    : QWindow(*new Qt3DQuickWindowPrivate, parent)
{
    Q_D(Qt3DQuickWindow);

    resize(1024, 768);
    setSurfaceType(SurfaceType::VulkanSurface);
    setTitle("Quartz Viewport");

    QObject::connect(this, &Qt3DQuickWindow::widthChanged, this, &Qt3DQuickWindow::updateCameraAspectRatio);
    QObject::connect(this, &Qt3DQuickWindow::heightChanged, this, &Qt3DQuickWindow::updateCameraAspectRatio);
    QObject::connect(this, &Qt3DQuickWindow::aboutToClose, d->m_raytraceAspect, &Qt3DRaytrace::QRaytraceAspect::suspendJobs);

    d->m_engine->aspectEngine()->registerAspect(d->m_raytraceAspect);
    d->m_engine->aspectEngine()->registerAspect(d->m_inputAspect);
    d->m_engine->aspectEngine()->registerAspect(d->m_logicAspect);

    Qt3DRaytrace::QRendererInterface *renderer = d->m_raytraceAspect->renderer();
    if(renderer) {
        renderer->setSurface(this);
    }
}

void Qt3DQuickWindow::registerAspect(Qt3DCore::QAbstractAspect *aspect)
{
    Q_ASSERT(!isVisible());
    Q_D(Qt3DQuickWindow);
    d->m_engine->aspectEngine()->registerAspect(aspect);
}

void Qt3DQuickWindow::registerAspect(const QString &name)
{
    Q_ASSERT(!isVisible());
    Q_D(Qt3DQuickWindow);
    d->m_engine->aspectEngine()->registerAspect(name);
}

void Qt3DQuickWindow::setSource(const QUrl &source)
{
    Q_D(Qt3DQuickWindow);
    d->m_source = source;
}

Qt3DCore::Quick::QQmlAspectEngine *Qt3DQuickWindow::engine() const
{
    Q_D(const Qt3DQuickWindow);
    return d->m_engine.get();
}

Qt3DQuickWindow::CameraAspectRatioMode Qt3DQuickWindow::cameraAspectRatioMode() const
{
    Q_D(const Qt3DQuickWindow);
    return d->m_cameraAspectRatioMode;
}

void Qt3DQuickWindow::setCameraAspectRatioMode(Qt3DQuickWindow::CameraAspectRatioMode mode)
{
    Q_D(Qt3DQuickWindow);
    if(d->m_cameraAspectRatioMode != mode) {
        d->m_cameraAspectRatioMode = mode;
        updateCameraAspectRatio();
        emit cameraAspectRatioModeChanged(mode);
    }
}

bool Qt3DQuickWindow::event(QEvent *event)
{
    Q_D(Qt3DQuickWindow);

    switch(event->type()) {
    case QEvent::Close:
        emit aboutToClose();
        break;
    case QEvent::PlatformSurface:
        if(static_cast<QPlatformSurfaceEvent*>(event)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
            if(d->m_raytraceAspect->renderer()) {
                d->m_raytraceAspect->renderer()->setSurface(nullptr);
            }
        }
        break;
    default:
        break;
    }

    return QWindow::event(event);
}

void Qt3DQuickWindow::showEvent(QShowEvent *event)
{
    Q_D(Qt3DQuickWindow);

    QWindow::showEvent(event);

    if(!d->m_initialized) {
        connect(d->m_engine.get(), &Qt3DCore::Quick::QQmlAspectEngine::sceneCreated, this, &Qt3DQuickWindow::sceneCreated);
        d->m_engine->setSource(d->m_source);
        // TODO: Setup incubation controller.
        d->m_initialized = true;
    }
}

void Qt3DQuickWindow::sceneCreated(QObject *root)
{
    Q_D(Qt3DQuickWindow);
    Q_ASSERT(root);

    Qt3DRaytrace::QCamera* camera = root->findChild<Qt3DRaytrace::QCamera*>();
    if(camera) {
        d->m_camera = camera;
        updateCameraAspectRatio();
    }

    Qt3DInput::QInputSettings *inputSettings = root->findChild<Qt3DInput::QInputSettings*>();
    if(inputSettings) {
        inputSettings->setEventSource(this);
    }
}

void Qt3DQuickWindow::updateCameraAspectRatio()
{
    Q_D(Qt3DQuickWindow);
    if(d->m_camera && d->m_cameraAspectRatioMode == AutomaticAspectRatio) {
        d->m_camera->setAspectRatio(float(width()) / float(height()));
    }
}

} // Quick
} // Qt3DRaytraceExtras
