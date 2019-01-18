/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qgeometry.h>
#include <Qt3DRaytrace/qgeometrydata.h>
#include <Qt3DCore/private/qnode_p.h>

namespace Qt3DRaytrace {

class QGeometryPrivate : public Qt3DCore::QNodePrivate
{
public:
    Q_DECLARE_PUBLIC(QGeometry)
    QGeometryData m_data;
};

} // Qt3DRaytrace
