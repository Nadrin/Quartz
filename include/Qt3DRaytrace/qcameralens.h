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
    Q_DECLARE_PRIVATE(QCameraLens)
    Q_PROPERTY(float aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(float fieldOfView READ fieldOfView WRITE setFieldOfView NOTIFY fieldOfViewChanged)
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
    Q_PROPERTY(float exposure READ exposure WRITE setExposure NOTIFY exposureChanged)
public:
    explicit QCameraLens(Qt3DCore::QNode *parent = nullptr);

    float aspectRatio() const;
    float fieldOfView() const;
    float gamma() const;
    float exposure() const;

public slots:
    void setAspectRatio(float aspectRatio);
    void setFieldOfView(float fov);
    void setGamma(float gamma);
    void setExposure(float exposure);

signals:
    void aspectRatioChanged(float aspectRatio);
    void fieldOfViewChanged(float fov);
    void gammaChanged(float gamma);
    void exposureChanged(float exposure);

protected:
    explicit QCameraLens(QCameraLensPrivate &dd, QNode *parent = nullptr);

private:
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DRaytrace
