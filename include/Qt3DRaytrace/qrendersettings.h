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

class QCamera;
class QRenderSettingsPrivate;

class QT3DRAYTRACESHARED_EXPORT QRenderSettings : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QRenderSettings)
    Q_PROPERTY(Qt3DRaytrace::QCamera* camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(int primarySamples READ primarySamples WRITE setPrimarySamples NOTIFY primarySamplesChanged)
    Q_PROPERTY(int secondarySamples READ secondarySamples WRITE setSecondarySamples NOTIFY secondarySamplesChanged)
    Q_PROPERTY(int minDepth READ minDepth WRITE setMinDepth NOTIFY minDepthChanged)
    Q_PROPERTY(int maxDepth READ maxDepth WRITE setMaxDepth NOTIFY maxDepthChanged)
    Q_PROPERTY(float directRadianceClamp READ directRadianceClamp WRITE setDirectRadianceClamp NOTIFY directRadianceClampChanged)
    Q_PROPERTY(float indirectRadianceClamp READ indirectRadianceClamp WRITE setIndirectRadianceClamp NOTIFY indirectRadianceClampChanged)
    Q_PROPERTY(QColor skyColor READ skyColor WRITE setSkyColor NOTIFY skyColorChanged)
    Q_PROPERTY(float skyIntensity READ skyIntensity WRITE setSkyIntensity NOTIFY skyIntensityChanged)
public:
    explicit QRenderSettings(Qt3DCore::QNode *parent = nullptr);

    QCamera *camera() const;
    int primarySamples() const;
    int secondarySamples() const;
    int minDepth() const;
    int maxDepth() const;
    float directRadianceClamp() const;
    float indirectRadianceClamp() const;
    QColor skyColor() const;
    float skyIntensity() const;

public slots:
    void setCamera(QCamera *camera);
    void setPrimarySamples(int primarySamples);
    void setSecondarySamples(int secondarySamples);
    void setMinDepth(int minDepth);
    void setMaxDepth(int maxDepth);
    void setDirectRadianceClamp(float clamp);
    void setIndirectRadianceClamp(float clamp);
    void setSkyColor(const QColor &skyColor);
    void setSkyIntensity(float skyIntensity);

signals:
    void cameraChanged(QCamera *camera);
    void primarySamplesChanged(int primarySamples);
    void secondarySamplesChanged(int secondarySamples);
    void minDepthChanged(int minDepth);
    void maxDepthChanged(int maxDepth);
    void directRadianceClampChanged(float clamp);
    void indirectRadianceClampChanged(float clamp);
    void skyColorChanged(const QColor &skyColor);
    void skyIntensityChanged(float skyIntensity);

protected:
    explicit QRenderSettings(QRenderSettingsPrivate &dd, QNode *parent = nullptr);

private:
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DRaytrace
