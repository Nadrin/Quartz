/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>

#include <Qt3DCore/QEntity>
#include <QVector3D>
#include <QQuaternion>

namespace Qt3DCore {
class QTransform;
};

namespace Qt3DRaytrace {

class QCameraLens;
class QCameraPrivate;

class QT3DRAYTRACESHARED_EXPORT QCamera : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(float pitch READ pitch WRITE setPitch NOTIFY pitchChanged)
    Q_PROPERTY(float yaw READ yaw WRITE setYaw NOTIFY yawChanged)
    Q_PROPERTY(float roll READ roll WRITE setRoll NOTIFY rollChanged)
    Q_PROPERTY(QVector3D lookAtTarget READ lookAtTarget WRITE setLookAtTarget NOTIFY lookAtTargetChanged)
    Q_PROPERTY(QVector3D lookAtUp READ lookAtUp WRITE setLookAtUp NOTIFY lookAtUpChanged)
    Q_PROPERTY(float aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(float fieldOfView READ fieldOfView WRITE setFieldOfView NOTIFY fieldOfViewChanged)
public:
    explicit QCamera(Qt3DCore::QNode *parent = nullptr);

    QCameraLens *lens() const;
    Qt3DCore::QTransform *transform() const;

    QVector3D position() const;
    QQuaternion rotation() const;

    float pitch() const;
    float yaw() const;
    float roll() const;

    QVector3D lookAtTarget() const;
    QVector3D lookAtUp() const;

    float aspectRatio() const;
    float fieldOfView() const;

public slots:
    void setPosition(const QVector3D &position);
    void setRotation(const QQuaternion &rotation);

    void setPitch(float pitch);
    void setYaw(float yaw);
    void setRoll(float roll);

    void setLookAtTarget(const QVector3D &lookAtTarget);
    void setLookAtUp(const QVector3D &lookAtUp);

    void setAspectRatio(float aspectRatio);
    void setFieldOfView(float fov);

signals:
    void positionChanged(const QVector3D &position);
    void rotationChanged(const QQuaternion &rotation);

    void pitchChanged(float pitch);
    void yawChanged(float yaw);
    void rollChanged(float roll);

    void lookAtTargetChanged(const QVector3D &lookAtTarget);
    void lookAtUpChanged(const QVector3D &lookAtUp);

    void aspectRatioChanged(float aspectRatio);
    void fieldOfViewChanged(float fov);

protected:
    explicit QCamera(QCameraPrivate &dd, Qt3DCore::QNode *parent = nullptr);
    Q_DECLARE_PRIVATE(QCamera)
};

} // Qt3DRaytrace
