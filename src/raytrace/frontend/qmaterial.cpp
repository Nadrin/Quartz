/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qmaterial_p.h>
#include <frontend/qabstracttexture_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QMaterial::QMaterial(Qt3DCore::QNode *parent)
    : QComponent(*new QMaterialPrivate, parent)
{}

QColor QMaterial::albedo() const
{
    Q_D(const QMaterial);
    return d->m_albedo;
}

float QMaterial::roughness() const
{
    Q_D(const QMaterial);
    return d->m_roughness;
}

float QMaterial::metalness() const
{
    Q_D(const QMaterial);
    return d->m_metalness;
}

QColor QMaterial::emission() const
{
    Q_D(const QMaterial);
    return d->m_emission;
}

float QMaterial::emissionIntensity() const
{
    Q_D(const QMaterial);
    return d->m_emissionIntensity;
}

QAbstractTexture *QMaterial::albedoTexture() const
{
    Q_D(const QMaterial);
    return d->m_albedoTexture;
}

QAbstractTexture *QMaterial::roughnessTexture() const
{
    Q_D(const QMaterial);
    return d->m_roughnessTexture;
}

QAbstractTexture *QMaterial::metalnessTexture() const
{
    Q_D(const QMaterial);
    return d->m_metalnessTexture;
}

void QMaterial::setAlbedo(const QColor &albedo)
{
    Q_D(QMaterial);
    if(d->m_albedo != albedo) {
        d->m_albedo = albedo;
        emit albedoChanged(albedo);
    }
}

void QMaterial::setRoughness(float rougness)
{
    Q_D(QMaterial);
    if(!qFuzzyCompare(d->m_roughness, rougness)) {
        d->m_roughness = rougness;
        emit roughnessChanged(rougness);
    }
}

void QMaterial::setMetalness(float metalness)
{
    Q_D(QMaterial);
    if(!qFuzzyCompare(d->m_metalness, metalness)) {
        d->m_metalness = metalness;
        emit metalnessChanged(metalness);
    }
}

void QMaterial::setEmission(const QColor &emission)
{
    Q_D(QMaterial);
    if(d->m_emission != emission) {
        d->m_emission = emission;
        emit emissionChanged(emission);
    }
}

void QMaterial::setEmissionIntensity(float intensity)
{
    Q_D(QMaterial);
    if(!qFuzzyCompare(d->m_emissionIntensity, intensity)) {
        d->m_emissionIntensity = intensity;
        emit emissionIntensityChanged(intensity);
    }
}

void QMaterial::setAlbedoTexture(QAbstractTexture *texture)
{
    Q_D(QMaterial);
    if(d->m_albedoTexture != texture) {
        d->m_albedoTexture = texture;
        emit albedoTextureChanged(texture);
    }
}

void QMaterial::setRoughnessTexture(QAbstractTexture *texture)
{
    Q_D(QMaterial);
    if(d->m_roughnessTexture != texture) {
        d->m_roughnessTexture = texture;
        emit roughnessTextureChanged(texture);
    }
}

void QMaterial::setMetalnessTexture(QAbstractTexture *texture)
{
    Q_D(QMaterial);
    if(d->m_metalnessTexture != texture) {
        d->m_metalnessTexture = texture;
        emit metalnessTextureChanged(texture);
    }
}

QMaterial::QMaterial(QMaterialPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{}

QNodeCreatedChangeBasePtr QMaterial::createNodeCreationChange() const
{
    Q_D(const QMaterial);

    auto creationChange = QNodeCreatedChangePtr<QMaterialData>::create(this);
    auto &data = creationChange->data;
    data.albedo = d->m_albedo;
    data.roughness = d->m_roughness;
    data.metalness = d->m_metalness;
    data.emission = d->m_emission;
    data.emissionIntensity = d->m_emissionIntensity;

    data.albedoTextureId = qIdForNode(d->m_albedoTexture);
    data.roughnessTextureId = qIdForNode(d->m_roughnessTexture);
    data.metalnessTextureId = qIdForNode(d->m_metalnessTexture);

    return creationChange;
}

} // Qt3DRaytrace
