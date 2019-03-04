/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <QVector3D>

namespace Qt3DRaytrace {

namespace Raytrace {
class Entity;
} // Raytrace

namespace Vulkan {

struct RenderParameters;
struct DisplayParameters;

class CameraManager
{
public:
    CameraManager();

    Raytrace::Entity *activeCamera() const;
    void setActiveCamera(Raytrace::Entity *activeCamera);

    void setDefaultParameters();
    void updateParameters();
    void applyRenderParameters(RenderParameters &params) const;
    void applyDisplayPrameters(DisplayParameters &params) const;

private:
    Raytrace::Entity *m_activeCamera;

    QVector3D m_position;
    QVector3D m_upVector;
    QVector3D m_rightVector;
    QVector3D m_forwardVector;
    float m_aspectRatio;
    float m_tanHalfFOV;
    float m_lensRadius;
    float m_lensFocalDistance;
    float m_invGamma;
    float m_exposure;
};

} // Vulkan
} // Qt3DRaytrace
