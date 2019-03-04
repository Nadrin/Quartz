/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qcamera_p.h>
#include <frontend/qcameralens_p.h>

#include <Qt3DCore/QTransform>

#include <cmath>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

template<typename T> void normalizeAngle(T &angle)
{
    T t = std::fmod(angle, 360.0f);
    angle = std::fmod(t + 360.0f, 360.0f);
}

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
                emit q->rotationPitchChanged(m_eulerAngles.x());
            }
            if(!qFuzzyCompare(eulerAngles.y(), m_eulerAngles.y())) {
                m_eulerAngles.setY(eulerAngles.y());
                emit q->rotationYawChanged(m_eulerAngles.y());
            }
            if(!qFuzzyCompare(eulerAngles.z(), m_eulerAngles.z())) {
                m_eulerAngles.setZ(eulerAngles.z());
                emit q->rotationRollChanged(m_eulerAngles.z());
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
    QObject::connect(d->m_lens, SIGNAL(diameterChanged(float)), this, SIGNAL(lensDiameterChanged(float)));
    QObject::connect(d->m_lens, SIGNAL(focalDistanceChanged(float)), this, SIGNAL(lensFocalDistanceChanged(float)));
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

float QCamera::rotationPitch() const
{
    Q_D(const QCamera);
    return d->m_eulerAngles.x();
}

float QCamera::rotationYaw() const
{
    Q_D(const QCamera);
    return d->m_eulerAngles.y();
}

float QCamera::rotationRoll() const
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

float QCamera::lensDiameter() const
{
    Q_D(const QCamera);
    return d->m_lens->diameter();
}

float QCamera::lensFocalDistance() const
{
    Q_D(const QCamera);
    return d->m_lens->focalDistance();
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

void QCamera::translate(const QVector3D &t)
{
    Q_D(QCamera);
    const QVector3D relativeTranslation = d->m_rotation.rotatedVector(t);
    setPosition(d->m_position + relativeTranslation);
}

void QCamera::translateWorld(const QVector3D &t)
{
    Q_D(QCamera);
    setPosition(d->m_position + t);
}

void QCamera::rotate(const QQuaternion &q)
{
    Q_D(QCamera);
    setRotation(q * d->m_rotation);
}

void QCamera::tilt(float angle)
{
    rotate(QQuaternion::fromEulerAngles(angle, 0.0f, 0.0f));
}

void QCamera::pan(float angle)
{
    rotate(QQuaternion::fromEulerAngles(0.0f, angle, 0.0f));
}

void QCamera::roll(float angle)
{
    rotate(QQuaternion::fromEulerAngles(0.0f, 0.0f, angle));
}

void QCamera::tiltWorld(float angle)
{
    setRotationPitch(rotationPitch() + angle);
}

void QCamera::panWorld(float angle)
{
    setRotationYaw(rotationYaw() + angle);
}

void QCamera::rollWorld(float angle)
{
    setRotationRoll(rotationRoll() + angle);
}

void QCamera::setLensFocalRatio(float fstop)
{
    if(fstop > 0.0f) {
        setLensDiameter(lensFocalDistance() / fstop);
    }
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

void QCamera::setRotationPitch(float rotationPitch)
{
    Q_D(QCamera);
    normalizeAngle(rotationPitch);
    if(!qFuzzyCompare(d->m_eulerAngles.x(), rotationPitch)) {
        d->m_eulerAngles.setX(rotationPitch);
        d->updateRotation(QQuaternion::fromEulerAngles(d->m_eulerAngles), false);
        emit rotationPitchChanged(rotationPitch);
    }
}

void QCamera::setRotationYaw(float rotationYaw)
{
    Q_D(QCamera);
    normalizeAngle(rotationYaw);
    if(!qFuzzyCompare(d->m_eulerAngles.y(), rotationYaw)) {
        d->m_eulerAngles.setY(rotationYaw);
        d->updateRotation(QQuaternion::fromEulerAngles(d->m_eulerAngles), false);
        emit rotationYawChanged(rotationYaw);
    }
}

void QCamera::setRotationRoll(float rotationRoll)
{
    Q_D(QCamera);
    normalizeAngle(rotationRoll);
    if(!qFuzzyCompare(d->m_eulerAngles.z(), rotationRoll)) {
        d->m_eulerAngles.setZ(rotationRoll);
        d->updateRotation(QQuaternion::fromEulerAngles(d->m_eulerAngles), false);
        emit rotationRollChanged(rotationRoll);
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

void QCamera::setLensDiameter(float diameter)
{
    Q_D(QCamera);
    d->m_lens->setDiameter(diameter);
}

void QCamera::setLensFocalDistance(float distance)
{
    Q_D(QCamera);
    d->m_lens->setFocalDistance(distance);
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
