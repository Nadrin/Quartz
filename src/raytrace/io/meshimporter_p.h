/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>

#include <Qt3DRaytrace/qgeometrydata.h>
#include <QUrl>

namespace Qt3DRaytrace {
namespace Raytrace {

class MeshImporter
{
public:
    virtual ~MeshImporter() = default;
    virtual bool import(const QUrl &url, QGeometryData &data) = 0;
};

} // Raytrace
} // Qt3DRaytrace
