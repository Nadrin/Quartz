/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <qraytraceaspect_p.h>
#include <backend/abstractrenderer_p.h>
#include <backend/rendersettings_p.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/private/qabstractframeadvanceservice_p.h>

#include <Qt3DRaytrace/qgeometry.h>
#include <Qt3DRaytrace/qgeometryrenderer.h>
#include <Qt3DRaytrace/qabstracttexture.h>
#include <Qt3DRaytrace/qtextureimage.h>
#include <Qt3DRaytrace/qmaterial.h>
#include <Qt3DRaytrace/qdistantlight.h>
#include <Qt3DRaytrace/qcamera.h>
#include <Qt3DRaytrace/qcameralens.h>
#include <Qt3DRaytrace/qrendersettings.h>

#include <renderers/vulkan/renderer.h>

#include <jobs/loadgeometryjob_p.h>
#include <jobs/loadtexturejob_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

Q_LOGGING_CATEGORY(logAspect, "raytrace.aspect")
Q_LOGGING_CATEGORY(logImport, "raytrace.import")

void QRaytraceAspectPrivate::registerBackendTypes()
{
    Q_Q(QRaytraceAspect);

    qRegisterMetaType<Qt3DRaytrace::QVertex>();
    qRegisterMetaType<Qt3DRaytrace::QTriangle>();
    qRegisterMetaType<Qt3DRaytrace::QGeometryData>();
    qRegisterMetaType<Qt3DRaytrace::QImageData>();
    qRegisterMetaType<Qt3DRaytrace::QImageDataPtr>();
    qRegisterMetaType<Qt3DRaytrace::QRenderImage>();

    qRegisterMetaType<Qt3DRaytrace::QCamera*>();
    qRegisterMetaType<Qt3DRaytrace::QGeometry*>();
    qRegisterMetaType<Qt3DRaytrace::QAbstractTexture*>();
    qRegisterMetaType<Qt3DRaytrace::QTextureImage*>();

    q->registerBackendType<Qt3DCore::QEntity>(QSharedPointer<Raytrace::EntityMapper>::create(m_nodeManagers.get(), m_renderer.get()));

    using TransformNodeMapper = Raytrace::BackendNodeMapper<Raytrace::Transform, Raytrace::TransformManager>;
    q->registerBackendType<Qt3DCore::QTransform>(QSharedPointer<TransformNodeMapper>::create(&m_nodeManagers->transformManager, m_renderer.get()));

    using CameraLensNodeMapper = Raytrace::BackendNodeMapper<Raytrace::CameraLens, Raytrace::CameraManager>;
    q->registerBackendType<QCameraLens>(QSharedPointer<CameraLensNodeMapper>::create(&m_nodeManagers->cameraManager, m_renderer.get()));

    using DistantLightNodeMapper = Raytrace::BackendNodeMapper<Raytrace::DistantLight, Raytrace::DistantLightManager>;
    q->registerBackendType<QDistantLight>(QSharedPointer<DistantLightNodeMapper>::create(&m_nodeManagers->distantLightManager, m_renderer.get()));

    q->registerBackendType<QGeometry>(QSharedPointer<Raytrace::GeometryNodeMapper>::create(&m_nodeManagers->geometryManager, m_renderer.get()));
    q->registerBackendType<QGeometryRenderer>(QSharedPointer<Raytrace::GeometryRendererNodeMapper>::create(&m_nodeManagers->geometryRendererManager, m_renderer.get()));
    q->registerBackendType<QAbstractTexture>(QSharedPointer<Raytrace::TextureNodeMapper>::create(&m_nodeManagers->textureManager, m_renderer.get()));
    q->registerBackendType<QTextureImage>(QSharedPointer<Raytrace::TextureImageNodeMapper>::create(&m_nodeManagers->textureImageManager, m_renderer.get()));
    q->registerBackendType<QMaterial>(QSharedPointer<Raytrace::MaterialNodeMapper>::create(&m_nodeManagers->materialManager, m_renderer.get()));

    q->registerBackendType<QRenderSettings>(QSharedPointer<Raytrace::RenderSettingsMapper>::create(m_renderer.get()));
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

QVector<QAspectJobPtr> QRaytraceAspectPrivate::createTextureJobs() const
{
    auto *textureManager = &m_nodeManagers->textureManager;
    auto dirtyTextures = textureManager->acquireDirtyComponents();

    QVector<QAspectJobPtr> textureJobs;
    textureJobs.reserve(dirtyTextures.size());
    for(const QNodeId &textureId : dirtyTextures) {
        Raytrace::HAbstractTexture handle = textureManager->lookupHandle(textureId);
        if(!handle.isNull()) {
            auto job = Raytrace::LoadTextureJobPtr::create(m_nodeManagers.get(), handle);
            textureJobs.append(job);
        }
    }
    return textureJobs;
}

QRaytraceAspect::QRaytraceAspect(QObject *parent)
    : QRaytraceAspect(*new QRaytraceAspectPrivate, parent)
{}

QSurface *QRaytraceAspect::surface() const
{
    Q_D(const QRaytraceAspect);
    return d->m_renderer ? d->m_renderer->surface() : nullptr;
}

void QRaytraceAspect::setSurface(QObject *surfaceObject)
{
    Q_D(QRaytraceAspect);
    if(d->m_renderer) {
        d->m_renderer->setSurface(surfaceObject);
    }
}

bool QRaytraceAspect::queryRenderStatistics(QRenderStatistics &statistics) const
{
    Q_D(const QRaytraceAspect);

    if(d->m_renderer) {
        statistics = d->m_renderer->statistics();
        return true;
    }
    return false;
}

void QRaytraceAspect::suspendJobs()
{
    Q_D(QRaytraceAspect);
    d->m_jobsSuspended = true;
}

void QRaytraceAspect::resumeJobs()
{
    Q_D(QRaytraceAspect);
    d->m_jobsSuspended = false;
}

void QRaytraceAspect::requestImage(QRenderImage type)
{
    QMetaObject::invokeMethod(this, "grabImage", Qt::AutoConnection,
                              Q_ARG(Qt3DRaytrace::QRenderImage, type));
}

void QRaytraceAspect::grabImage(QRenderImage type)
{
    Q_D(QRaytraceAspect);
    if(d->m_renderer) {
        QImageDataPtr image(new QImageData);
        *image = d->m_renderer->grabImage(type);
        if(!image->data.isEmpty()) {
            emit imageReady(type, image);
        }
    }
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

    if(d->m_jobsSuspended) {
        return jobs;
    }

    jobs.append(d->createGeometryRendererJobs());
    jobs.append(d->createTextureJobs());
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
