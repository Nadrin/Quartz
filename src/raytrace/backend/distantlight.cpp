/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <backend/distantlight_p.h>
#include <backend/managers_p.h>
#include <frontend/qdistantlight_p.h>

#include <Qt3DCore/QPropertyUpdatedChange>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

DistantLight::DistantLight()
    : BackendNode(ReadOnly)
{}

void DistantLight::sceneChangeEvent(const QSceneChangePtr &change)
{
    if(change->type() == PropertyUpdated) {
        QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(change);
        const auto propertyName = propertyChange->propertyName();
        if(propertyName == QByteArrayLiteral("color")) {
            m_color = propertyChange->value().value<QColor>();
        }
        else if(propertyName == QByteArrayLiteral("intensity")) {
            m_intensity = propertyChange->value().value<float>();
        }
        else if(propertyName == QByteArrayLiteral("direction")) {
            m_direction = propertyChange->value().value<QVector3D>();
        }
    }

    markDirty(AbstractRenderer::LightDirty);
    BackendNode::sceneChangeEvent(change);
}

void DistantLight::initializeFromPeer(const QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<QNodeCreatedChange<QLightData>>(change);
    const auto &data = typedChange->data;

    m_color = data.color;
    m_intensity = data.intensity;
    m_direction = data.direction;

    markDirty(AbstractRenderer::LightDirty);
}

} // Raytrace
} // Qt3DRaytrace
