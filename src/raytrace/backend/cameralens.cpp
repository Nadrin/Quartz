/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <backend/cameralens_p.h>
#include <frontend/qcameralens_p.h>

#include <Qt3DCore/QPropertyUpdatedChange>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

CameraLens::CameraLens()
    : BackendNode(BackendNode::ReadOnly)
{}

void CameraLens::sceneChangeEvent(const QSceneChangePtr &change)
{
    if(change->type() == PropertyUpdated) {
        QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(change);
        if(propertyChange->propertyName() == QByteArrayLiteral("fieldOfView")) {
            m_fieldOfView = propertyChange->value().value<float>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("aspectRatio")) {
            m_aspectRatio = propertyChange->value().value<float>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("diameter")) {
            m_diameter = propertyChange->value().value<float>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("focalDistance")) {
            m_focalDistance = propertyChange->value().value<float>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("gamma")) {
            m_gamma = propertyChange->value().value<float>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("exposure")) {
            m_exposure = propertyChange->value().value<float>();
        }
        markDirty(AbstractRenderer::CameraDirty);
    }
    BackendNode::sceneChangeEvent(change);
}

void CameraLens::initializeFromPeer(const QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QCameraLensData>>(change);
    const auto &data = typedChange->data;

    m_aspectRatio = data.aspectRatio;
    m_fieldOfView = data.fieldOfView;
    m_diameter = data.diameter;
    m_focalDistance = data.focalDistance;
    m_gamma = data.gamma;
    m_exposure = data.exposure;

    markDirty(AbstractRenderer::CameraDirty);
}

} // Raytrace
} // Qt3DRaytrace
