/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qrendersettings_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QRenderSettings::QRenderSettings(QNode *parent)
    : QRenderSettings(*new QRenderSettingsPrivate, parent)
{}

QRenderSettings::QRenderSettings(QRenderSettingsPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{}

int QRenderSettings::primarySamples() const
{
    Q_D(const QRenderSettings);
    return d->m_data.primarySamples;
}

int QRenderSettings::secondarySamples() const
{
    Q_D(const QRenderSettings);
    return d->m_data.secondarySamples;
}

int QRenderSettings::maxDepth() const
{
    Q_D(const QRenderSettings);
    return d->m_data.maxDepth;
}

QColor QRenderSettings::skyColor() const
{
    Q_D(const QRenderSettings);
    return d->m_data.skyColor;
}

float QRenderSettings::skyIntensity() const
{
    Q_D(const QRenderSettings);
    return d->m_data.skyIntensity;
}

void QRenderSettings::setPrimarySamples(int primarySamples)
{
    Q_D(QRenderSettings);
    primarySamples = std::max(primarySamples, 1);
    if(d->m_data.primarySamples != primarySamples) {
        d->m_data.primarySamples = primarySamples;
        emit primarySamplesChanged(primarySamples);
    }
}

void QRenderSettings::setSecondarySamples(int secondarySamples)
{
    Q_D(QRenderSettings);
    secondarySamples = std::max(secondarySamples, 1);
    if(d->m_data.secondarySamples != secondarySamples) {
        d->m_data.secondarySamples = secondarySamples;
        emit secondarySamplesChanged(secondarySamples);
    }
}

void QRenderSettings::setMaxDepth(int maxDepth)
{
    Q_D(QRenderSettings);
    maxDepth = std::max(maxDepth, 1);
    if(d->m_data.maxDepth != maxDepth) {
        d->m_data.maxDepth = maxDepth;
        emit maxDepthChanged(maxDepth);
    }
}

void QRenderSettings::setSkyColor(const QColor &skyColor)
{
    Q_D(QRenderSettings);
    if(d->m_data.skyColor != skyColor) {
        d->m_data.skyColor = skyColor;
        emit skyColorChanged(skyColor);
    }
}

void QRenderSettings::setSkyIntensity(float skyIntensity)
{
    Q_D(QRenderSettings);
    skyIntensity = std::max(skyIntensity, 0.0f);
    if(!qFuzzyCompare(d->m_data.skyIntensity, skyIntensity)) {
        d->m_data.skyIntensity = skyIntensity;
        emit skyIntensityChanged(skyIntensity);
    }
}

QNodeCreatedChangeBasePtr QRenderSettings::createNodeCreationChange() const
{
    Q_D(const QRenderSettings);

    auto creationChange = QNodeCreatedChangePtr<QRenderSettingsData>::create(this);
    creationChange->data = d->m_data;
    return creationChange;
}

} // Qt3DRaytrace
