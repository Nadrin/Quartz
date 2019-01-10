/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <backend/transform_p.h>
#include <Qt3DCore/QPropertyUpdatedChange>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

Transform::Transform()
{
    m_transform.scale = { 1.0f, 1.0f, 1.0f };
}

void Transform::initializeFromPeer(const QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QTransformData>>(change);
    m_transform = typedChange->data;
    updateTransformMatrix();
}

void Transform::sceneChangeEvent(const QSceneChangePtr &change)
{
    if(change->type() == PropertyUpdated) {
        const QPropertyUpdatedChangePtr &propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(change);
        if(propertyChange->propertyName() == QByteArrayLiteral("translation")) {
            m_transform.translation = propertyChange->value().value<QVector3D>();
            updateTransformMatrix();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("rotation")) {
            m_transform.rotation = propertyChange->value().value<QQuaternion>();
            updateTransformMatrix();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("scale3D")) {
            m_transform.scale = propertyChange->value().value<QVector3D>();
            updateTransformMatrix();
        }
    }
}

void Transform::updateTransformMatrix()
{
    QMatrix4x4 m;
    m.translate(m_transform.translation);
    m.rotate(m_transform.rotation);
    m.scale(m_transform.scale);
    m_transformMatrix = Matrix4x4(m);
}

} // Raytrace
} // Qt3DRaytrace

