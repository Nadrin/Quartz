/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qdistantlight.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/private/qtypedpropertyupdatechange_p.h>

namespace Qt3DRaytrace {

struct QLightData
{
    QColor color = Qt::GlobalColor::white;
    float intensity = 1.0f;
    QVector3D direction{0.0f, -1.0f, 0.0f};
};

class QDistantLightPrivate : public Qt3DCore::QComponentPrivate
{
public:
    Q_DECLARE_PUBLIC(QDistantLight)
    QLightData m_data;
};

} // Qt3DRaytrace
