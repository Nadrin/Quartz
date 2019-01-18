/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <backend/backendnode_p.h>

#include <Qt3DRaytrace/qgeometrydata.h>

namespace Qt3DRaytrace {
namespace Raytrace {

class Geometry : public BackendNode
{
public:
    const QGeometryData &data() const { return m_data; }
    const QVector<QVertex> &vertices() const { return m_data.vertices; }
    const QVector<QTriangle> &faces() const { return m_data.faces; }

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;

    QGeometryData m_data;
};

} // Raytrace
} // Qt3DRaytrace
