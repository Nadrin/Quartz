/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qcameralens.h>
#include <Qt3DCore/private/qcomponent_p.h>

namespace Qt3DRaytrace {

struct QCameraLensData
{
    float aspectRatio = 1.0f;
    float fieldOfView = 90.0f;
    float diameter = 0.0f;
    float focalDistance = 1.0f;
    float gamma = 2.2f;
    float exposure = 1.0f;
    float tonemapFactor = 1.0f;
};

class QCameraLensPrivate : public Qt3DCore::QComponentPrivate
{
public:
    Q_DECLARE_PUBLIC(QCameraLens)
    QCameraLensData m_data;
};

} // Qt3DRaytrace
