/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qrendersettings_p.h>
#include <frontend/qcamera_p.h>
#include <frontend/qabstracttexture_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QRenderSettings::QRenderSettings(QNode *parent)
    : QRenderSettings(*new QRenderSettingsPrivate, parent)
{}

QCamera *QRenderSettings::camera() const
{
    Q_D(const QRenderSettings);
    return d->m_camera;
}

QRenderSettings::QRenderSettings(QRenderSettingsPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{}

int QRenderSettings::primarySamples() const
{
    Q_D(const QRenderSettings);
    return d->m_settings.primarySamples;
}

int QRenderSettings::secondarySamples() const
{
    Q_D(const QRenderSettings);
    return d->m_settings.secondarySamples;
}

int QRenderSettings::minDepth() const
{
    Q_D(const QRenderSettings);
    return d->m_settings.minDepth;
}

int QRenderSettings::maxDepth() const
{
    Q_D(const QRenderSettings);
    return d->m_settings.maxDepth;
}

float QRenderSettings::directRadianceClamp() const
{
    Q_D(const QRenderSettings);
    return d->m_settings.directRadianceClamp;
}

float QRenderSettings::indirectRadianceClamp() const
{
    Q_D(const QRenderSettings);
    return d->m_settings.indirectRadianceClamp;
}

QColor QRenderSettings::skyColor() const
{
    Q_D(const QRenderSettings);
    return d->m_settings.skyColor;
}

float QRenderSettings::skyIntensity() const
{
    Q_D(const QRenderSettings);
    return d->m_settings.skyIntensity;
}

QAbstractTexture *QRenderSettings::skyTexture() const
{
    Q_D(const QRenderSettings);
    return d->m_skyTexture;
}

void QRenderSettings::setCamera(QCamera *camera)
{
    Q_D(QRenderSettings);
    if(d->m_camera != camera) {
        if(d->m_camera) {
            d->unregisterDestructionHelper(d->m_camera);
        }
        d->m_camera = camera;
        if(d->m_camera) {
            if(!d->m_camera->parent()) {
                d->m_camera->setParent(this);
            }
            d->registerDestructionHelper(d->m_camera, &QRenderSettings::setCamera, d->m_camera);
        }
        d->m_settings.cameraId = qIdForNode(camera);
        emit cameraChanged(camera);
    }
}

void QRenderSettings::setPrimarySamples(int primarySamples)
{
    Q_D(QRenderSettings);
    primarySamples = std::max(primarySamples, 1);
    if(d->m_settings.primarySamples != primarySamples) {
        d->m_settings.primarySamples = primarySamples;
        emit primarySamplesChanged(primarySamples);
    }
}

void QRenderSettings::setSecondarySamples(int secondarySamples)
{
    Q_D(QRenderSettings);
    secondarySamples = std::max(secondarySamples, 1);
    if(d->m_settings.secondarySamples != secondarySamples) {
        d->m_settings.secondarySamples = secondarySamples;
        emit secondarySamplesChanged(secondarySamples);
    }
}

void QRenderSettings::setMinDepth(int minDepth)
{
    Q_D(QRenderSettings);
    minDepth = std::max(minDepth, 0);
    if(d->m_settings.minDepth != minDepth) {
        d->m_settings.minDepth = minDepth;
        emit minDepthChanged(minDepth);
    }
}

void QRenderSettings::setMaxDepth(int maxDepth)
{
    Q_D(QRenderSettings);
    maxDepth = std::max(maxDepth, 0);
    if(d->m_settings.maxDepth != maxDepth) {
        d->m_settings.maxDepth = maxDepth;
        emit maxDepthChanged(maxDepth);
    }
}

void QRenderSettings::setDirectRadianceClamp(float clamp)
{
    Q_D(QRenderSettings);
    if(!qFuzzyCompare(d->m_settings.directRadianceClamp, clamp)) {
        d->m_settings.directRadianceClamp = clamp;
        emit directRadianceClampChanged(clamp);
    }
}

void QRenderSettings::setIndirectRadianceClamp(float clamp)
{
    Q_D(QRenderSettings);
    if(!qFuzzyCompare(d->m_settings.indirectRadianceClamp, clamp)) {
        d->m_settings.indirectRadianceClamp = clamp;
        emit indirectRadianceClampChanged(clamp);
    }
}

void QRenderSettings::setSkyColor(const QColor &skyColor)
{
    Q_D(QRenderSettings);
    if(d->m_settings.skyColor != skyColor) {
        d->m_settings.skyColor = skyColor;
        emit skyColorChanged(skyColor);
    }
}

void QRenderSettings::setSkyIntensity(float skyIntensity)
{
    Q_D(QRenderSettings);
    skyIntensity = std::max(skyIntensity, 0.0f);
    if(!qFuzzyCompare(d->m_settings.skyIntensity, skyIntensity)) {
        d->m_settings.skyIntensity = skyIntensity;
        emit skyIntensityChanged(skyIntensity);
    }
}

void QRenderSettings::setSkyTexture(QAbstractTexture *texture)
{
    Q_D(QRenderSettings);
    if(d->m_skyTexture != texture) {
        if(d->m_skyTexture) {
            d->unregisterDestructionHelper(d->m_skyTexture);
        }
        d->m_skyTexture = texture;
        if(d->m_skyTexture) {
            if(!d->m_skyTexture->parent()) {
                d->m_skyTexture->setParent(this);
            }
            d->registerDestructionHelper(d->m_skyTexture, &QRenderSettings::setSkyTexture, d->m_skyTexture);
        }
        d->m_settings.skyTextureId = qIdForNode(texture);
        emit skyTextureChanged(texture);
    }
}

QNodeCreatedChangeBasePtr QRenderSettings::createNodeCreationChange() const
{
    Q_D(const QRenderSettings);

    auto creationChange = QNodeCreatedChangePtr<QRenderSettingsData>::create(this);
    creationChange->data = d->m_settings;
    return creationChange;
}

} // Qt3DRaytrace
