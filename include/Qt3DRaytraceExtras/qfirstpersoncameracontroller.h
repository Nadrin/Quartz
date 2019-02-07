/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytraceExtras/qt3draytraceextras_global.h>
#include <Qt3DCore/QEntity>

namespace Qt3DRaytrace {
class QCamera;
} // Qt3DRaytrace

namespace Qt3DRaytraceExtras {

class QFirstPersonCameraControllerPrivate;

class QT3DRAYTRACEEXTRASSHARED_EXPORT QFirstPersonCameraController : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QFirstPersonCameraController)
    Q_PROPERTY(Qt3DRaytrace::QCamera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(float moveSpeed READ moveSpeed WRITE setMoveSpeed NOTIFY moveSpeedChanged)
    Q_PROPERTY(float lookSpeed READ lookSpeed WRITE setLookSpeed NOTIFY lookSpeedChanged)
    Q_PROPERTY(float acceleration READ acceleration WRITE setAcceleration NOTIFY accelerationChanged)
    Q_PROPERTY(float deceleration READ deceleration WRITE setDeceleration NOTIFY decelerationChanged)
public:
    explicit QFirstPersonCameraController(Qt3DCore::QNode *parent = nullptr);

    Qt3DRaytrace::QCamera *camera() const;
    float moveSpeed() const;
    float lookSpeed() const;
    float acceleration() const;
    float deceleration() const;

public slots:
    void setCamera(Qt3DRaytrace::QCamera *camera);
    void setMoveSpeed(float moveSpeed);
    void setLookSpeed(float lookSpeed);
    void setAcceleration(float acceleration);
    void setDeceleration(float deceleration);

signals:
    void cameraChanged(Qt3DRaytrace::QCamera *camera);
    void moveSpeedChanged(float moveSpeed);
    void lookSpeedChanged(float lookSpeed);
    void accelerationChanged(float acceleration);
    void decelerationChanged(float deceleration);

protected:
    explicit QFirstPersonCameraController(QFirstPersonCameraControllerPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private slots:
    void frameAction(float dt);
};

} // Qt3DRaytraceExtras
