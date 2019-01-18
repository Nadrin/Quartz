/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <backend/geometry_p.h>
#include <frontend/qgeometry_p.h>

#include <Qt3DCore/QPropertyUpdatedChange>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

void Geometry::sceneChangeEvent(const QSceneChangePtr &change)
{
    if(change->type() == PropertyUpdated) {
        QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(change);
        if(propertyChange->propertyName() == QByteArrayLiteral("data")) {
            m_data  = propertyChange->value().value<QGeometryData>();
            markDirty(AbstractRenderer::GeometryDirty);
        }
    }
    BackendNode::sceneChangeEvent(change);
}

void Geometry::initializeFromPeer(const QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QGeometryData>>(change);
    m_data  = typedChange->data;
    markDirty(AbstractRenderer::GeometryDirty);
}

} // Raytrace
} // Qt3DRaytrace
