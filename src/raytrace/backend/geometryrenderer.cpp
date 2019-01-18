/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <backend/geometryrenderer_p.h>
#include <backend/managers_p.h>
#include <frontend/qgeometryrenderer_p.h>

#include <Qt3DCore/QPropertyUpdatedChange>
#include <QCoreApplication>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

GeometryRenderer::GeometryRenderer()
    : BackendNode(ReadWrite)
    , m_manager(nullptr)
{}

void GeometryRenderer::setManager(GeometryRendererManager *manager)
{
    Q_ASSERT(manager);
    m_manager = manager;
}

void GeometryRenderer::sceneChangeEvent(const QSceneChangePtr &change)
{
    if(change->type() == PropertyUpdated) {
        QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(change);
        const auto propertyName = propertyChange->propertyName();
        if(propertyName == QByteArrayLiteral("geometry")) {
            m_geometryId = propertyChange->value().value<QNodeId>();
        }
        else if(propertyName == QByteArrayLiteral("geometryFactory")) {
            m_geometryFactory = propertyChange->value().value<QGeometryFactoryPtr>();
            if(m_geometryFactory && m_manager) {
                m_manager->markComponentDirty(peerId());
            }
        }
    }

    markDirty(AbstractRenderer::GeometryDirty);
    BackendNode::sceneChangeEvent(change);
}

void GeometryRenderer::loadGeometry()
{
    Q_ASSERT(m_geometryFactory);

    std::unique_ptr<QGeometry> geometry(m_geometryFactory->create());
    if(geometry) {
        geometry->moveToThread(QCoreApplication::instance()->thread());

        auto change = QGeometryChangePtr::create(peerId());
        change->setDeliveryFlags(QSceneChange::Nodes);
        change->setPropertyName("geometry");
        change->data = std::move(geometry);
        notifyObservers(change);
    }
}

void GeometryRenderer::initializeFromPeer(const QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<QNodeCreatedChange<QGeometryRendererData>>(change);
    const auto &data = typedChange->data;

    m_geometryId = data.geometryId;
    m_geometryFactory = data.geometryFactory;
    if(m_geometryFactory && m_manager) {
        m_manager->markComponentDirty(peerId());
    }

    markDirty(AbstractRenderer::GeometryDirty);
}

} // Raytrace
} // Qt3DRaytrace
