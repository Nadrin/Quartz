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

#include <Qt3DRaytrace/qgeometry.h>
#include <Qt3DRaytrace/qgeometryrenderer.h>
#include <Qt3DRaytrace/qmaterial.h>
#include <Qt3DRaytrace/qcamera.h>
#include <Qt3DRaytrace/qcameralens.h>

#include <renderers/vulkan/renderer.h>

#include <jobs/loadgeometryjob_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

Q_LOGGING_CATEGORY(logAspect, "raytrace.aspect")

QRaytraceAspectPrivate::QRaytraceAspectPrivate()
{}

void QRaytraceAspectPrivate::registerBackendTypes()
{
    Q_Q(QRaytraceAspect);

    qRegisterMetaType<Qt3DRaytrace::QVertex>();
    qRegisterMetaType<Qt3DRaytrace::QTriangle>();
    qRegisterMetaType<Qt3DRaytrace::QGeometryData>();

    q->registerBackendType<Qt3DCore::QEntity>(QSharedPointer<Raytrace::EntityMapper>::create(m_nodeManagers.get(), m_renderer.get()));

    using TransformNodeMapper = Raytrace::BackendNodeMapper<Raytrace::Transform, Raytrace::TransformManager>;
    q->registerBackendType<Qt3DCore::QTransform>(QSharedPointer<TransformNodeMapper>::create(&m_nodeManagers->transformManager, m_renderer.get()));

    using CameraLensNodeMapper = Raytrace::BackendNodeMapper<Raytrace::CameraLens, Raytrace::CameraManager>;
    q->registerBackendType<QCameraLens>(QSharedPointer<CameraLensNodeMapper>::create(&m_nodeManagers->cameraManager, m_renderer.get()));

    q->registerBackendType<QGeometry>(QSharedPointer<Raytrace::GeometryNodeMapper>::create(&m_nodeManagers->geometryManager, m_renderer.get()));
    q->registerBackendType<QGeometryRenderer>(QSharedPointer<Raytrace::GeometryRendererNodeMapper>::create(&m_nodeManagers->geometryRendererManager, m_renderer.get()));
    q->registerBackendType<QMaterial>(QSharedPointer<Raytrace::MaterialNodeMapper>::create(&m_nodeManagers->materialManager, m_renderer.get()));
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

QVector<QAspectJobPtr> QRaytraceAspectPrivate::createGeometryRendererJobs() const
{
    auto *geometryRendererManager = &m_nodeManagers->geometryRendererManager;
    auto dirtyGeometryRenderers = geometryRendererManager->acquireDirtyComponents();

    QVector<QAspectJobPtr> geometryRendererJobs;
    geometryRendererJobs.reserve(dirtyGeometryRenderers.size());
    for(const QNodeId &geometryRendererId : dirtyGeometryRenderers) {
        Raytrace::HGeometryRenderer handle = geometryRendererManager->lookupHandle(geometryRendererId);
        if(!handle.isNull()) {
            auto job = Raytrace::LoadGeometryJobPtr::create(m_nodeManagers.get(), handle);
            geometryRendererJobs.append(job);
        }
    }
    return geometryRendererJobs;
}

QRaytraceAspect::QRaytraceAspect(QObject *parent)
    : QRaytraceAspect(*new QRaytraceAspectPrivate, parent)
{}

QRendererInterface *QRaytraceAspect::renderer() const
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
    jobs.append(d->createGeometryRendererJobs());
    if(d->m_renderer) {
        jobs.append(d->m_renderer->jobsToExecute(time));
    }
    return jobs;
}

void QRaytraceAspect::onRegistered()
{
    Q_D(QRaytraceAspect);

    d->m_nodeManagers.reset(new Raytrace::NodeManagers);

    // TODO: Make renderer configurable.
    d->m_renderer.reset(new Vulkan::Renderer);
    d->m_renderer->setNodeManagers(d->m_nodeManagers.get());

    d->updateServiceProviders();
    d->registerBackendTypes();
}

void QRaytraceAspect::onUnregistered()
{
    Q_D(QRaytraceAspect);

    d->m_renderer.reset();
    d->m_nodeManagers.reset();
}

void QRaytraceAspect::onEngineStartup()
{
    Q_D(QRaytraceAspect);

    Q_ASSERT(d->m_nodeManagers);
    Q_ASSERT(d->m_renderer);

    Raytrace::Entity *rootEntity = d->m_nodeManagers->entityManager.lookupResource(rootEntityId());
    if(rootEntity) {
        d->m_renderer->setSceneRoot(rootEntity);
    }

    if(!d->m_renderer->initialize()) {
        qCWarning(logAspect) << "Failed to initialize renderer";
    }
}

void QRaytraceAspect::onEngineShutdown()
{
    Q_D(QRaytraceAspect);

    Q_ASSERT(d->m_renderer);
    d->m_renderer->shutdown();
}

} // Qt3DRaytrace
