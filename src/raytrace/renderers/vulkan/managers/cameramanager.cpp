/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/managers/cameramanager.h>
#include <renderers/vulkan/glsl.h>

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
    m_lensRadius = 0.0f;
    m_lensFocalDistance = 1.0f;
    m_invGamma = 1.0f / 2.2f;
    m_exposure = 1.0f;
    m_tonemapFactor = 1.0f;
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
    if(lens) {
        m_aspectRatio = lens->aspectRatio();
        m_tanHalfFOV = std::tan(0.5f * qDegreesToRadians(lens->fieldOfView()));
        m_lensRadius = 0.5f * lens->diameter();
        m_lensFocalDistance = lens->focalDistance();
        m_invGamma = 1.0f / lens->gamma();
        m_exposure = lens->exposure();
        m_tonemapFactor = lens->tonemapFactor();
    }
}

void CameraManager::applyRenderParameters(RenderParameters &params) const
{
    params.cameraPositionAspect.data[0] = m_position.x();
    params.cameraPositionAspect.data[1] = m_position.y();
    params.cameraPositionAspect.data[2] = m_position.z();
    params.cameraPositionAspect.data[3] = m_aspectRatio;

    params.cameraUpVectorTanHalfFOV.data[0] = m_upVector.x();
    params.cameraUpVectorTanHalfFOV.data[1] = m_upVector.y();
    params.cameraUpVectorTanHalfFOV.data[2] = m_upVector.z();
    params.cameraUpVectorTanHalfFOV.data[3] = m_tanHalfFOV;

    params.cameraRightVectorLensR[0] = m_rightVector.x();
    params.cameraRightVectorLensR[1] = m_rightVector.y();
    params.cameraRightVectorLensR[2] = m_rightVector.z();
    params.cameraRightVectorLensR[3] = m_lensRadius;

    params.cameraForwardVectorLensF[0] = m_forwardVector.x();
    params.cameraForwardVectorLensF[1] = m_forwardVector.y();
    params.cameraForwardVectorLensF[2] = m_forwardVector.z();
    params.cameraForwardVectorLensF[3] = m_lensFocalDistance;
}

void CameraManager::applyDisplayPrameters(DisplayParameters &params) const
{
    params.invGamma = m_invGamma;
    params.exposure = m_exposure;
    params.tonemapFactorSq = m_tonemapFactor * m_tonemapFactor;
}

} // Vulkan
} // Qt3DRaytrace
