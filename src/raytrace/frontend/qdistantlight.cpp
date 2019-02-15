/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qdistantlight_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QDistantLight::QDistantLight(QNode *parent)
    : QComponent(*new QDistantLightPrivate, parent)
{}

QColor QDistantLight::color() const
{
    Q_D(const QDistantLight);
    return d->m_data.color;
}

float QDistantLight::intensity() const
{
    Q_D(const QDistantLight);
    return d->m_data.intensity;
}

QVector3D QDistantLight::direction() const
{
    Q_D(const QDistantLight);
    return d->m_data.direction;
}

void QDistantLight::setColor(const QColor &color)
{
    Q_D(QDistantLight);
    if(d->m_data.color != color) {
        d->m_data.color = color;
        emit colorChanged(color);
    }
}

void QDistantLight::setIntensity(float intensity)
{
    Q_D(QDistantLight);
    if(!qFuzzyCompare(d->m_data.intensity, intensity)) {
        d->m_data.intensity = intensity;
        emit intensityChanged(intensity);
    }
}

void QDistantLight::setDirection(const QVector3D &direction)
{
    Q_D(QDistantLight);
    const QVector3D normalizedDirection = direction.normalized();
    if(!qFuzzyCompare(d->m_data.direction, normalizedDirection)) {
        d->m_data.direction = normalizedDirection;
        emit directionChanged(normalizedDirection);
    }
}

QDistantLight::QDistantLight(QDistantLightPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{}

QNodeCreatedChangeBasePtr QDistantLight::createNodeCreationChange() const
{
    Q_D(const QDistantLight);

    auto creationChange = QNodeCreatedChangePtr<QLightData>::create(this);
    creationChange->data = d->m_data;
    return creationChange;
}

} // Qt3DRaytrace
