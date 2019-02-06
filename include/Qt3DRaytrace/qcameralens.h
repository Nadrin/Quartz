/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>

#include <Qt3DCore/QComponent>

namespace Qt3DRaytrace {

class QCameraLensPrivate;

class QT3DRAYTRACESHARED_EXPORT QCameraLens : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(float aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(float fieldOfView READ fieldOfView WRITE setFieldOfView NOTIFY fieldOfViewChanged)
public:
    explicit QCameraLens(Qt3DCore::QNode *parent = nullptr);

    float aspectRatio() const;
    float fieldOfView() const;

public slots:
    void setAspectRatio(float aspectRatio);
    void setFieldOfView(float fov);

signals:
    void aspectRatioChanged(float aspectRatio);
    void fieldOfViewChanged(float fov);

protected:
    explicit QCameraLens(QCameraLensPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QCameraLens)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DRaytrace
