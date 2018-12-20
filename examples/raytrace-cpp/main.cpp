/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <QGuiApplication>
#include <Qt3DRaytraceExtras/qt3dwindow.h>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    return app.exec();
}
