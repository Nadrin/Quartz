/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qrendersettings.h>
#include <Qt3DCore/QNodeId>
#include <Qt3DCore/private/qcomponent_p.h>

namespace Qt3DRaytrace {

struct QRenderSettingsData
{
    Qt3DCore::QNodeId cameraId;

    int primarySamples = 1;
    int secondarySamples = 1;
    int minDepth = 2;
    int maxDepth = 4;

    QColor skyColor = Qt::GlobalColor::black;
    float skyIntensity = 1.0f;
};

class QRenderSettingsPrivate : public Qt3DCore::QComponentPrivate
{
public:
    Q_DECLARE_PUBLIC(QRenderSettings)
    QRenderSettingsData m_settings;
    QCamera *m_camera = nullptr;
};

} // Qt3DRaytrace
