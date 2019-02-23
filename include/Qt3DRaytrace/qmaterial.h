/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>
#include <Qt3DRaytrace/qabstracttexture.h>

#include <Qt3DCore/QComponent>
#include <QColor>

namespace Qt3DRaytrace {

class QMaterialPrivate;

class QT3DRAYTRACESHARED_EXPORT QMaterial : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(QColor albedo READ albedo WRITE setAlbedo NOTIFY albedoChanged)
    Q_PROPERTY(float roughness READ roughness WRITE setRoughness NOTIFY roughnessChanged)
    Q_PROPERTY(float metalness READ metalness WRITE setMetalness NOTIFY metalnessChanged)
    Q_PROPERTY(QColor emission READ emission WRITE setEmission NOTIFY emissionChanged)
    Q_PROPERTY(float emissionIntensity READ emissionIntensity WRITE setEmissionIntensity NOTIFY emissionIntensityChanged)
    Q_PROPERTY(Qt3DRaytrace::QAbstractTexture* albedoTexture READ albedoTexture WRITE setAlbedoTexture NOTIFY albedoTextureChanged)
    Q_PROPERTY(Qt3DRaytrace::QAbstractTexture* roughnessTexture READ roughnessTexture WRITE setRoughnessTexture NOTIFY roughnessTextureChanged)
    Q_PROPERTY(Qt3DRaytrace::QAbstractTexture* metalnessTexture READ metalnessTexture WRITE setMetalnessTexture NOTIFY metalnessTextureChanged)
public:
    explicit QMaterial(Qt3DCore::QNode *parent = nullptr);

    QColor albedo() const;
    float roughness() const;
    float metalness() const;
    QColor emission() const;
    float emissionIntensity() const;

    QAbstractTexture *albedoTexture() const;
    QAbstractTexture *roughnessTexture() const;
    QAbstractTexture *metalnessTexture() const;

public slots:
    void setAlbedo(const QColor &albedo);
    void setRoughness(float rougness);
    void setMetalness(float metalness);
    void setEmission(const QColor &emission);
    void setEmissionIntensity(float intensity);

    void setAlbedoTexture(QAbstractTexture *texture);
    void setRoughnessTexture(QAbstractTexture *texture);
    void setMetalnessTexture(QAbstractTexture *texture);

signals:
    void albedoChanged(const QColor &albedo);
    void roughnessChanged(float roughness);
    void metalnessChanged(float metalness);
    void emissionChanged(const QColor &emission);
    void emissionIntensityChanged(float intensity);

    void albedoTextureChanged(QAbstractTexture *texture);
    void roughnessTextureChanged(QAbstractTexture *texture);
    void metalnessTextureChanged(QAbstractTexture *texture);

protected:
    explicit QMaterial(QMaterialPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QMaterial)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DRaytrace
