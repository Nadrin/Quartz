/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qcamera_p.h>
#include <frontend/qcameralens_p.h>

#include <Qt3DCore/QTransform>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QCameraPrivate::QCameraPrivate()
    : m_lens(new QCameraLens)
    , m_transform(new Qt3DCore::QTransform)
    , m_lookAtTarget(QVector3D(0.0f, 0.0f, -1.0f))
    , m_lookAtUp(QVector3D(0.0f, 1.0f, 0.0f))
{}

void QCameraPrivate::updateRotation(const QQuaternion &rotation, bool updateEulerAngles)
{
    Q_Q(QCamera);
    if(!qFuzzyCompare(rotation, m_rotation)) {
        m_rotation = rotation;
        m_transform->setRotation(m_rotation);
        emit q->rotationChanged(m_rotation);

        if(updateEulerAngles) {
            QVector3D eulerAngles = m_rotation.toEulerAngles();
            if(!qFuzzyCompare(eulerAngles.x(), m_eulerAngles.x())) {
                m_eulerAngles.setX(eulerAngles.x());
                emit q->pitchChanged(m_eulerAngles.x());
            }
            if(!qFuzzyCompare(eulerAngles.y(), m_eulerAngles.y())) {
                m_eulerAngles.setY(eulerAngles.y());
                emit q->yawChanged(m_eulerAngles.y());
            }
            if(!qFuzzyCompare(eulerAngles.z(), m_eulerAngles.z())) {
                m_eulerAngles.setZ(eulerAngles.z());
                emit q->rollChanged(m_eulerAngles.z());
            }
        }
    }
}

void QCameraPrivate::updateLookAtRotation()
{
    const QVector3D viewDirection = (m_lookAtTarget - m_position).normalized();
    updateRotation(QQuaternion::fromDirection(-viewDirection, m_lookAtUp), true);
}

QCamera::QCamera(QNode *parent)
    : QCamera(*new QCameraPrivate, parent)
{}

QCamera::QCamera(QCameraPrivate &dd, QNode *parent)
    : QEntity(dd, parent)
{
    Q_D(QCamera);

    QObject::connect(d->m_lens, SIGNAL(aspectRatioChanged(float)), this, SIGNAL(aspectRatioChanged(float)));
    QObject::connect(d->m_lens, SIGNAL(fieldOfViewChanged(float)), this, SIGNAL(fieldOfViewChanged(float)));
    QObject::connect(d->m_lens, SIGNAL(exposureChanged(float)), this, SIGNAL(exposureChanged(float)));
    QObject::connect(d->m_lens, SIGNAL(gammaChanged(float)), this, SIGNAL(gammaChanged(float)));

    addComponent(d->m_lens);
    addComponent(d->m_transform);
}

QCameraLens *QCamera::lens() const
{
    Q_D(const QCamera);
    return d->m_lens;
}

Qt3DCore::QTransform *QCamera::transform() const
{
    Q_D(const QCamera);
    return d->m_transform;
}

QVector3D QCamera::position() const
{
    Q_D(const QCamera);
    return d->m_position;
}

QQuaternion QCamera::rotation() const
{
    Q_D(const QCamera);
    return d->m_rotation;
}

float QCamera::pitch() const
{
    Q_D(const QCamera);
    return d->m_eulerAngles.x();
}

float QCamera::yaw() const
{
    Q_D(const QCamera);
    return d->m_eulerAngles.y();
}

float QCamera::roll() const
{
    Q_D(const QCamera);
    return d->m_eulerAngles.z();
}

QVector3D QCamera::lookAtTarget() const
{
    Q_D(const QCamera);
    return d->m_lookAtTarget;
}

QVector3D QCamera::lookAtUp() const
{
    Q_D(const QCamera);
    return d->m_lookAtUp;
}

float QCamera::aspectRatio() const
{
    Q_D(const QCamera);
    return d->m_lens->aspectRatio();
}

float QCamera::fieldOfView() const
{
    Q_D(const QCamera);
    return d->m_lens->fieldOfView();
}

float QCamera::gamma() const
{
    Q_D(const QCamera);
    return d->m_lens->gamma();
}

float QCamera::exposure() const
{
    Q_D(const QCamera);
    return d->m_lens->exposure();
}

void QCamera::setPosition(const QVector3D &position)
{
    Q_D(QCamera);
    if(!qFuzzyCompare(d->m_position, position)) {
        d->m_position = position;
        d->m_transform->setTranslation(position);
        emit positionChanged(position);
    }
}

void QCamera::setRotation(const QQuaternion &rotation)
{
    Q_D(QCamera);
    d->updateRotation(rotation, true);
}

void QCamera::setPitch(float pitch)
{
    Q_D(QCamera);
    if(!qFuzzyCompare(d->m_eulerAngles.x(), pitch)) {
        d->m_eulerAngles.setX(pitch);
        d->updateRotation(QQuaternion::fromEulerAngles(d->m_eulerAngles), false);
        emit pitchChanged(pitch);
    }
}

void QCamera::setYaw(float yaw)
{
    Q_D(QCamera);
    if(!qFuzzyCompare(d->m_eulerAngles.y(), yaw)) {
        d->m_eulerAngles.setY(yaw);
        d->updateRotation(QQuaternion::fromEulerAngles(d->m_eulerAngles), false);
        emit yawChanged(yaw);
    }
}

void QCamera::setRoll(float roll)
{
    Q_D(QCamera);
    if(!qFuzzyCompare(d->m_eulerAngles.z(), roll)) {
        d->m_eulerAngles.setZ(roll);
        d->updateRotation(QQuaternion::fromEulerAngles(d->m_eulerAngles), false);
        emit rollChanged(roll);
    }
}

void QCamera::setLookAtTarget(const QVector3D &lookAtTarget)
{
    Q_D(QCamera);
    d->m_lookAtTarget = lookAtTarget;
    d->updateLookAtRotation();
    emit lookAtTargetChanged(d->m_lookAtTarget);
}

void QCamera::setLookAtUp(const QVector3D &lookAtUp)
{
    Q_D(QCamera);
    d->m_lookAtUp = lookAtUp.normalized();
    d->updateLookAtRotation();
    emit lookAtUpChanged(d->m_lookAtUp);
}

void QCamera::setAspectRatio(float aspectRatio)
{
    Q_D(QCamera);
    d->m_lens->setAspectRatio(aspectRatio);
}

void QCamera::setFieldOfView(float fov)
{
    Q_D(QCamera);
    d->m_lens->setFieldOfView(fov);
}

void QCamera::setGamma(float gamma)
{
    Q_D(QCamera);
    d->m_lens->setGamma(gamma);
}

void QCamera::setExposure(float exposure)
{
    Q_D(QCamera);
    d->m_lens->setExposure(exposure);
}

} // Qt3DRaytrace
