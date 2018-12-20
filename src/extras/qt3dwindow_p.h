/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytraceExtras/qt3dwindow.h>
#include <QtGui/private/qwindow_p.h>

namespace Qt3DRaytraceExtras {

class Qt3DWindowPrivate : public QWindowPrivate
{
public:
    Qt3DWindowPrivate();

    Q_DECLARE_PUBLIC(Qt3DWindow)
};

} // Qt3DRaytraceExtras
