/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <backend/backendnode_p.h>
#include <backend/types_p.h>

#include <QColor>
#include <QVector3D>

namespace Qt3DRaytrace {
namespace Raytrace {

class DistantLightManager;

class DistantLight : public BackendNode
{
public:
    DistantLight();

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

    LinearColor radiance() const
    {
        return LinearColor(m_color, m_intensity);
    }
    QVector3D direction() const
    {
        return m_direction;
    }

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;

    QColor m_color;
    float m_intensity;
    QVector3D m_direction;
};

} // Raytrace
} // Qt3DRaytrace
