/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qgeometryrenderer_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QGeometryRenderer::QGeometryRenderer(QNode *parent)
    : QComponent(*new QGeometryRendererPrivate, parent)
{}

QGeometryRenderer::QGeometryRenderer(QGeometryRendererPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{}

QGeometry *QGeometryRenderer::geometry() const
{
    Q_D(const QGeometryRenderer);
    return d->m_geometry;
}

QGeometryFactoryPtr QGeometryRenderer::geometryFactory() const
{
    Q_D(const QGeometryRenderer);
    return d->m_geometryFactory;
}

void QGeometryRenderer::setGeometryFactory(const QGeometryFactoryPtr &factory)
{
    Q_D(QGeometryRenderer);
    d->m_geometryFactory = factory;
    if(d->m_changeArbiter) {
        auto change = QPropertyUpdatedChangePtr::create(d->m_id);
        change->setPropertyName("geometryFactory");
        change->setValue(QVariant::fromValue(d->m_geometryFactory));
        d->notifyObservers(change);
    }
}

void QGeometryRenderer::setGeometry(QGeometry *geometry)
{
    Q_D(QGeometryRenderer);
    if(d->m_geometry != geometry) {
        if(d->m_geometry) {
            d->unregisterDestructionHelper(d->m_geometry);
        }
        d->m_geometry = geometry;
        if(d->m_geometry) {
            if(!d->m_geometry->parent()) {
                d->m_geometry->setParent(this);
            }
            d->registerDestructionHelper(d->m_geometry, &QGeometryRenderer::setGeometry, d->m_geometry);
        }
        emit geometryChanged(d->m_geometry);
    }
}

void QGeometryRenderer::sceneChangeEvent(const QSceneChangePtr &change)
{
    if(change->type() == PropertyUpdated) {
        auto propertyChange = qSharedPointerCast<QStaticPropertyUpdatedChangeBase>(change);
        if(propertyChange->propertyName() == QByteArrayLiteral("geometry")) {
            auto typedChange = qSharedPointerCast<QGeometryChange>(change);
            setGeometry(typedChange->data.release());
        }
    }
}

QNodeCreatedChangeBasePtr QGeometryRenderer::createNodeCreationChange() const
{
    Q_D(const QGeometryRenderer);
    auto creationChange = QNodeCreatedChangePtr<QGeometryRendererData>::create(this);
    auto &data = creationChange->data;
    data.geometryId = qIdForNode(d->m_geometry);
    data.geometryFactory = d->m_geometryFactory;
    return creationChange;
}

} // Qt3DRaytrace
