/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <qraytraceaspect_p.h>
#include <backend/abstractrenderer_p.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/private/qabstractframeadvanceservice_p.h>

#include <renderers/vulkan/renderer.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

Q_LOGGING_CATEGORY(logAspect, "raytrace.aspect")

QRaytraceAspectPrivate::QRaytraceAspectPrivate()
{}

void QRaytraceAspectPrivate::registerBackendTypes()
{
    Q_Q(QRaytraceAspect);

    q->registerBackendType<Qt3DCore::QEntity>(QSharedPointer<Raytrace::EntityMapper>::create(m_nodeManagers.get(), m_renderer.get()));

    using TransformNodeMapper = Raytrace::BackendNodeMapper<Raytrace::Transform, Raytrace::TransformManager>;
    q->registerBackendType<Qt3DCore::QTransform>(QSharedPointer<TransformNodeMapper>::create(&m_nodeManagers->transformManager, m_renderer.get()));
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

QRendererInterface *QRaytraceAspect::rendererInterface() const
{
    Q_D(const QRaytraceAspect);
    return d->m_renderer.get();
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
    if(d->m_renderer) {
        jobs.append(d->m_renderer->renderJobs());
    }
    return jobs;
}

void QRaytraceAspect::onRegistered()
{
    Q_D(QRaytraceAspect);

    // TODO: Make renderer configurable.
    d->m_renderer.reset(new Vulkan::Renderer);

    d->m_nodeManagers.reset(new Raytrace::NodeManagers);
    d->updateServiceProviders();
    d->registerBackendTypes();
}

void QRaytraceAspect::onUnregistered()
{
    Q_D(QRaytraceAspect);
    d->m_nodeManagers.reset();
    d->m_renderer.reset();
}

void QRaytraceAspect::onEngineStartup()
{
    Q_D(QRaytraceAspect);

    Q_ASSERT(d->m_nodeManagers);
    Raytrace::Entity *rootEntity = d->m_nodeManagers->entityManager.lookupResource(rootEntityId());
    if(rootEntity) {
        d->m_renderer->setSceneRoot(rootEntity);
    }
}

} // Qt3DRaytrace
