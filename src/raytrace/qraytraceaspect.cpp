/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <Qt3DRaytrace/qraytraceaspect.h>
#include <qraytraceaspect_p.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/private/qabstractframeadvanceservice_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

Q_LOGGING_CATEGORY(logAspect, "raytrace.aspect")

QRaytraceAspectPrivate::QRaytraceAspectPrivate()
    : m_renderer(nullptr)
{}

void QRaytraceAspectPrivate::registerBackendTypes()
{
    Q_Q(QRaytraceAspect);

    q->registerBackendType<Qt3DCore::QEntity>(QSharedPointer<Raytrace::EntityMapper>::create(m_nodeManagers.get(), m_renderer));

    using TransformNodeMapper = Raytrace::BackendNodeMapper<Raytrace::Transform, Raytrace::TransformManager>;
    q->registerBackendType<Qt3DCore::QTransform>(QSharedPointer<TransformNodeMapper>::create(&m_nodeManagers->transformManager, m_renderer));
}

void QRaytraceAspectPrivate::updateServiceProviders()
{
    if(!m_aspectManager) {
        return;
    }
    if(m_renderer) {
        QAbstractFrameAdvanceService *advanceService = m_renderer->frameAdvanceService();
        if(advanceService) {
            services()->registerServiceProvider(Qt3DCore::QServiceLocator::FrameAdvanceService, advanceService);
        }
    }
}

QRaytraceAspect::QRaytraceAspect(QObject *parent)
    : QRaytraceAspect(*new QRaytraceAspectPrivate, parent)
{}

QAbstractRenderer *QRaytraceAspect::renderer() const
{
    Q_D(const QRaytraceAspect);
    return d->m_renderer;
}

void QRaytraceAspect::setRenderer(QAbstractRenderer *renderer)
{
    Q_D(QRaytraceAspect);
    d->m_renderer = renderer;
}

QRaytraceAspect::QRaytraceAspect(QRaytraceAspectPrivate &dd, QObject *parent)
    : QAbstractAspect(dd, parent)
{
    setObjectName(QStringLiteral("Raytrace Aspect"));
}

QVector<QAspectJobPtr> QRaytraceAspect::jobsToExecute(qint64 time)
{
    Q_D(QRaytraceAspect);

    QVector<QAspectJobPtr> jobs;
    return jobs;
}

void QRaytraceAspect::onRegistered()
{
    Q_D(QRaytraceAspect);
    if(!d->m_renderer) {
        qCWarning(logAspect) << "No renderer set: cannot properly initialize aspect!";
        return;
    }

    d->m_nodeManagers.reset(new Raytrace::NodeManagers);
    d->updateServiceProviders();
    d->registerBackendTypes();
}

void QRaytraceAspect::onUnregistered()
{
    Q_D(QRaytraceAspect);
    d->m_nodeManagers.reset();
}

} // Qt3DRaytrace
