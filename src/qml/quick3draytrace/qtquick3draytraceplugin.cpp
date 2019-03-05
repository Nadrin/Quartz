/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <qtquick3draytraceplugin.h>
#include <qqml.h>

#include <Qt3DRaytrace/qt3draytracecontext.h>
#include <Qt3DRaytrace/qgeometry.h>
#include <Qt3DRaytrace/qgeometryrenderer.h>
#include <Qt3DRaytrace/qmesh.h>
#include <Qt3DRaytrace/qtextureimage.h>
#include <Qt3DRaytrace/qabstracttexture.h>
#include <Qt3DRaytrace/qtexture.h>
#include <Qt3DRaytrace/qmaterial.h>
#include <Qt3DRaytrace/qdistantlight.h>
#include <Qt3DRaytrace/qcamera.h>
#include <Qt3DRaytrace/qcameralens.h>
#include <Qt3DRaytrace/qrendersettings.h>

void Qt3DQuick3DRaytracePlugin::registerTypes(const char *uri)
{
    // Context
    qmlRegisterSingletonType<Qt3DRaytrace::Quick::Qt3DRaytraceContext>(uri, 1, 0, "Qt3DRaytrace", [](QQmlEngine*, QJSEngine*) -> QObject* {
        return new Qt3DRaytrace::Quick::Qt3DRaytraceContext;
    });

    // Geometry
    qmlRegisterType<Qt3DRaytrace::QGeometry>(uri, 1, 0, "Geometry");
    qmlRegisterType<Qt3DRaytrace::QGeometryRenderer>(uri, 1, 0, "GeometryRenderer");
    qmlRegisterType<Qt3DRaytrace::QMesh>(uri, 1, 0, "Mesh");

    // Textures
    qmlRegisterType<Qt3DRaytrace::QTextureImage>(uri, 1, 0, "TextureImage");
    qmlRegisterType<Qt3DRaytrace::QAbstractTexture>(uri, 1, 0, "AbstractTexture");
    qmlRegisterType<Qt3DRaytrace::QTexture>(uri, 1, 0, "Texture");

    // Material
    qmlRegisterType<Qt3DRaytrace::QMaterial>(uri, 1, 0, "Material");

    // Lights
    qmlRegisterType<Qt3DRaytrace::QDistantLight>(uri, 1, 0, "DistantLight");

    // Camera
    qmlRegisterType<Qt3DRaytrace::QCamera>(uri, 1, 0, "Camera");
    qmlRegisterType<Qt3DRaytrace::QCameraLens>(uri, 1, 0, "CameraLens");

    // Settings
    qmlRegisterType<Qt3DRaytrace::QRenderSettings>(uri, 1, 0, "RenderSettings");

    // Module
    qmlRegisterModule(uri, 1, 0);
}
