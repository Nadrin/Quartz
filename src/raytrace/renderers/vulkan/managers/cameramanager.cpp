/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/managers/cameramanager.h>
#include <renderers/vulkan/shaders/types.glsl.h>

#include <backend/entity_p.h>
#include <backend/transform_p.h>
#include <backend/cameralens_p.h>

#include <QtMath>
#include <QMatrix4x4>

namespace Qt3DRaytrace {
namespace Vulkan {

// Use right-handed coordinate system.
// Up vector is flipped since screen coordinates increase from top to bottom.
static constexpr QVector3D IdentityUpVector(0.0f, -1.0f, 0.0f);
static constexpr QVector3D IdentityRightVector(1.0f, 0.0f, 0.0f);
static constexpr QVector3D IdentityForwardVector(0.0f, 0.0f, -1.0f);

static constexpr float DefaultFOV = 90.0f;

CameraManager::CameraManager()
    : m_activeCamera(nullptr)
{
    setDefaultParameters();
}

Raytrace::Entity *CameraManager::activeCamera() const
{
    return m_activeCamera;
}

void CameraManager::setActiveCamera(Raytrace::Entity *activeCamera)
{
    m_activeCamera = activeCamera;
}

void CameraManager::setDefaultParameters()
{
    m_position = QVector3D(0.0f, 0.0f, 0.0f);
    m_upVector = IdentityUpVector;
    m_rightVector = IdentityRightVector;
    m_forwardVector = IdentityForwardVector;
    m_aspectRatio = 1.0f;
    m_tanHalfFOV = std::tan(0.5f * qDegreesToRadians(DefaultFOV));
}

void CameraManager::updateParameters()
{
    if(!m_activeCamera || !m_activeCamera->isCamera()) {
        setDefaultParameters();
        return;
    }

    const QMatrix4x4 worldTransformMatrix = m_activeCamera->worldTransformMatrix.toQMatrix4x4();

    m_position = QVector3D(worldTransformMatrix.column(3));
    m_upVector = worldTransformMatrix.mapVector(IdentityUpVector);
    m_rightVector = worldTransformMatrix.mapVector(IdentityRightVector);
    m_forwardVector = worldTransformMatrix.mapVector(IdentityForwardVector);

    const Raytrace::CameraLens *lens = m_activeCamera->cameraLensComponent();
    Q_ASSERT(lens);
    m_aspectRatio = lens->aspectRatio();
    m_tanHalfFOV = std::tan(0.5f * qDegreesToRadians(lens->fieldOfView()));
}

void CameraManager::writeParameters(RenderParameters &params) const
{
    params.cameraPositionAndAspect.data[0] = m_position.x();
    params.cameraPositionAndAspect.data[1] = m_position.y();
    params.cameraPositionAndAspect.data[2] = m_position.z();
    params.cameraPositionAndAspect.data[3] = m_aspectRatio;

    params.cameraUpVectorAndTanHalfFOV.data[0] = m_upVector.x();
    params.cameraUpVectorAndTanHalfFOV.data[1] = m_upVector.y();
    params.cameraUpVectorAndTanHalfFOV.data[2] = m_upVector.z();
    params.cameraUpVectorAndTanHalfFOV.data[3] = m_tanHalfFOV;

    params.cameraRightVector[0] = m_rightVector.x();
    params.cameraRightVector[1] = m_rightVector.y();
    params.cameraRightVector[2] = m_rightVector.z();

    params.cameraForwardVector[0] = m_forwardVector.x();
    params.cameraForwardVector[1] = m_forwardVector.y();
    params.cameraForwardVector[2] = m_forwardVector.z();
}

} // Vulkan
} // Qt3DRaytrace
