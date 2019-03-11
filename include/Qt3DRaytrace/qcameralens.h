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
    Q_PROPERTY(float diameter READ diameter WRITE setDiameter NOTIFY diameterChanged)
    Q_PROPERTY(float focalDistance READ focalDistance WRITE setFocalDistance NOTIFY focalDistanceChanged)
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
    Q_PROPERTY(float exposure READ exposure WRITE setExposure NOTIFY exposureChanged)
    Q_PROPERTY(float tonemapFactor READ tonemapFactor WRITE setTonemapFactor NOTIFY tonemapFactorChanged)
public:
    explicit QCameraLens(Qt3DCore::QNode *parent = nullptr);

    float aspectRatio() const;
    float fieldOfView() const;
    float diameter() const;
    float focalDistance() const;
    float gamma() const;
    float exposure() const;
    float tonemapFactor() const;

public slots:
    void setAspectRatio(float aspectRatio);
    void setFieldOfView(float fov);
    void setDiameter(float diameter);
    void setFocalDistance(float distance);
    void setGamma(float gamma);
    void setExposure(float exposure);
    void setTonemapFactor(float factor);

signals:
    void aspectRatioChanged(float aspectRatio);
    void fieldOfViewChanged(float fov);
    void diameterChanged(float diameter);
    void focalDistanceChanged(float distance);
    void gammaChanged(float gamma);
    void exposureChanged(float exposure);
    void tonemapFactorChanged(float factor);

protected:
    explicit QCameraLens(QCameraLensPrivate &dd, QNode *parent = nullptr);

private:
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DRaytrace
