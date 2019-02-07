/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <qtquick3draytraceextrasplugin.h>
#include <qqml.h>

#include <Qt3DRaytraceExtras/qfirstpersoncameracontroller.h>

void Qt3DQuick3DRaytraceExtrasPlugin::registerTypes(const char *uri)
{
    // Camera controllers
    qmlRegisterType<Qt3DRaytraceExtras::QFirstPersonCameraController>(uri, 1, 0, "FirstPersonCameraController");

    // Module
    qmlRegisterModule(uri, 1, 0);
}
