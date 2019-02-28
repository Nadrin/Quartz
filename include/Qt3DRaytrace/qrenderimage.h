/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>
#include <QtCore/qmetatype.h>

namespace Qt3DRaytrace {

enum class QRenderImage
{
    HDR,
    FinalLDR,
};

struct QRenderStatistics
{
    double cpuFrameTime;
    double gpuFrameTime;
    double totalRenderTime;
    unsigned int numFramesRendered;
};

} // Qt3DRaytrace

Q_DECLARE_METATYPE(Qt3DRaytrace::QRenderImage)
