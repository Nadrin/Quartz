/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytraceExtras/qt3draytraceextras_global.h>
#include <QtGui/QWindow>

namespace Qt3DRaytraceExtras {

class Qt3DWindowPrivate;

class QT3DRAYTRACEEXTRASSHARED_EXPORT Qt3DWindow : public QWindow
{
    Q_OBJECT
public:
    explicit Qt3DWindow(QScreen *screen = nullptr);
    virtual ~Qt3DWindow();

private:
    Q_DECLARE_PRIVATE(Qt3DWindow)
};

} // Qt3DRaytraceExtras
