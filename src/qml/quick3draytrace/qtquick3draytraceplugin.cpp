/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <qtquick3draytraceplugin.h>
#include <qqml.h>

#include <Qt3DRaytrace/qgeometry.h>
#include <Qt3DRaytrace/qgeometryrenderer.h>
#include <Qt3DRaytrace/qmesh.h>
#include <Qt3DRaytrace/qmaterial.h>

void Qt3DQuick3DRaytracePlugin::registerTypes(const char *uri)
{
    // Geometry
    qmlRegisterType<Qt3DRaytrace::QGeometry>(uri, 1, 0, "Geometry");
    qmlRegisterType<Qt3DRaytrace::QGeometryRenderer>(uri, 1, 0, "GeometryRenderer");
    qmlRegisterType<Qt3DRaytrace::QMesh>(uri, 1, 0, "Mesh");

    // Material
    qmlRegisterType<Qt3DRaytrace::QMaterial>(uri, 1, 0, "Material");

    // Module
    qmlRegisterModule(uri, 1, 0);
}
