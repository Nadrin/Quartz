/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qcameralens_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QCameraLens::QCameraLens(QNode *parent)
    : QCameraLens(*new QCameraLensPrivate, parent)
{}

QCameraLens::QCameraLens(QCameraLensPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{}

float QCameraLens::aspectRatio() const
{
    Q_D(const QCameraLens);
    return d->m_data.aspectRatio;
}

float QCameraLens::fieldOfView() const
{
    Q_D(const QCameraLens);
    return d->m_data.fieldOfView;
}

void QCameraLens::setAspectRatio(float aspectRatio)
{
    Q_D(QCameraLens);
    if(!qFuzzyCompare(d->m_data.aspectRatio, aspectRatio)) {
        d->m_data.aspectRatio = aspectRatio;
        emit aspectRatioChanged(aspectRatio);
    }
}

void QCameraLens::setFieldOfView(float fov)
{
    Q_D(QCameraLens);
    if(!qFuzzyCompare(d->m_data.fieldOfView, fov)) {
        d->m_data.fieldOfView = fov;
        emit fieldOfViewChanged(fov);
    }
}

QNodeCreatedChangeBasePtr QCameraLens::createNodeCreationChange() const
{
    Q_D(const QCameraLens);

    auto creationChange = QNodeCreatedChangePtr<QCameraLensData>::create(this);
    creationChange->data = d->m_data;
    return creationChange;
}

} // Qt3DRaytrace
