/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>

#include <Qt3DCore/QComponent>
#include <QColor>

namespace Qt3DRaytrace {

class QMaterialPrivate;

class QT3DRAYTRACESHARED_EXPORT QMaterial : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(QColor albedo READ albedo WRITE setAlbedo NOTIFY albedoChanged)
    Q_PROPERTY(QColor emission READ emission WRITE setEmission NOTIFY emissionChanged)
    Q_PROPERTY(float emissionIntensity READ emissionIntensity WRITE setEmissionIntensity NOTIFY emissionIntensityChanged)
public:
    explicit QMaterial(Qt3DCore::QNode *parent = nullptr);

    QColor albedo() const;
    QColor emission() const;
    float emissionIntensity() const;

public slots:
    void setAlbedo(const QColor &albedo);
    void setEmission(const QColor &emission);
    void setEmissionIntensity(float intensity);

signals:
    void albedoChanged(const QColor &albedo);
    void emissionChanged(const QColor &emission);
    void emissionIntensityChanged(float intensity);

protected:
    explicit QMaterial(QMaterialPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QMaterial)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DRaytrace
