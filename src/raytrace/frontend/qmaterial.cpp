/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qmaterial_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QMaterial::QMaterial(Qt3DCore::QNode *parent)
    : QComponent(*new QMaterialPrivate, parent)
{}

QColor QMaterial::albedo() const
{
    Q_D(const QMaterial);
    return d->m_data.albedo;
}

QColor QMaterial::emission() const
{
    Q_D(const QMaterial);
    return d->m_data.emission;
}

float QMaterial::emissionPower() const
{
    Q_D(const QMaterial);
    return d->m_data.emissionPower;
}

void QMaterial::setAlbedo(const QColor &albedo)
{
    Q_D(QMaterial);
    if(d->m_data.albedo != albedo) {
        d->m_data.albedo = albedo;
        emit albedoChanged(albedo);
    }
}

void QMaterial::setEmission(const QColor &emission)
{
    Q_D(QMaterial);
    if(d->m_data.emission != emission) {
        d->m_data.emission = emission;
        emit emissionChanged(emission);
    }
}

void QMaterial::setEmissionPower(float power)
{
    Q_D(QMaterial);
    if(!qFuzzyCompare(d->m_data.emissionPower, power)) {
        d->m_data.emissionPower = power;
        emit emissionPowerChanged(power);
    }
}

QMaterial::QMaterial(QMaterialPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{}

QNodeCreatedChangeBasePtr QMaterial::createNodeCreationChange() const
{
    Q_D(const QMaterial);

    auto creationChange = QNodeCreatedChangePtr<QMaterialData>::create(this);
    creationChange->data = d->m_data;
    return creationChange;
}

} // Qt3DRaytrace
