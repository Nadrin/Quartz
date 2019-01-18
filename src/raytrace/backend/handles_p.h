/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <Qt3DCore/private/qhandle_p.h>

namespace Qt3DRaytrace {
namespace Raytrace {

using HEntity = Qt3DCore::QHandle<class Entity>;
using HTransform = Qt3DCore::QHandle<class Transform>;
using HGeometry = Qt3DCore::QHandle<class Geometry>;
using HGeometryRenderer = Qt3DCore::QHandle<class GeometryRenderer>;

} // Raytrace
} // Qt3DRaytrace
