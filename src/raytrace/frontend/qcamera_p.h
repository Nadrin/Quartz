/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qcamera.h>
#include <Qt3DCore/private/qentity_p.h>

namespace Qt3DCore {
class QTransform;
} // Qt3DCore

namespace Qt3DRaytrace {

class QCameraLens;

class QCameraPrivate : public Qt3DCore::QEntityPrivate
{
public:
    QCameraPrivate();
    Q_DECLARE_PUBLIC(QCamera)

    void updateRotation(const QQuaternion &rotation, bool updateEulerAngles);
    void updateLookAtRotation();

    QCameraLens *m_lens;
    Qt3DCore::QTransform *m_transform;

    QVector3D m_position;
    QQuaternion m_rotation;
    QVector3D m_eulerAngles;
    QVector3D m_lookAtTarget;
    QVector3D m_lookAtUp;
};

} // Qt3DRaytrace
