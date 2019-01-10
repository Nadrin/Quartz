/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <backend/backendnode_p.h>

#include <QVector3D>
#include <QQuaternion>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/private/matrix4x4_p.h>
#include <Qt3DCore/private/qtransform_p.h>

namespace Qt3DRaytrace {
namespace Raytrace {

class Transform : public BackendNode
{
public:
    Transform();

    Matrix4x4 transformMatrix() const { return m_transformMatrix; }

protected:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) override;

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;
    void updateTransformMatrix();

    Qt3DCore::QTransformData m_transform;
    Matrix4x4 m_transformMatrix;
};

} // Raytrace
} // Qt3DRaytrace
