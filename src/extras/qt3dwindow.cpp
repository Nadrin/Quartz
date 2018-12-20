/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <qt3dwindow_p.h>

namespace Qt3DRaytraceExtras {

Qt3DWindow::Qt3DWindow(QScreen *screen)
    : QWindow(*new Qt3DWindowPrivate, nullptr)
{
}

Qt3DWindow::~Qt3DWindow()
{

}

Qt3DRaytraceExtras::Qt3DWindowPrivate::Qt3DWindowPrivate()
{}

} // Qt3DRaytraceExtras
