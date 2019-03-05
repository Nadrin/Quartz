/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <Qt3DRaytrace/qt3draytracecontext.h>
#include <Qt3DRaytrace/qcolorspace.h>

namespace Qt3DRaytrace {
namespace Quick {

Qt3DRaytraceContext::Qt3DRaytraceContext(QObject *parent)
    : QObject(parent)
{}

QColor Qt3DRaytraceContext::lrgba(qreal r, qreal g, qreal b, qreal a) const
{
    return Qt3DRaytrace::to_sRgb(QColor::fromRgbF(r, g, b, a));
}

QColor Qt3DRaytraceContext::srgba(qreal r, qreal g, qreal b, qreal a) const
{
    return QColor::fromRgbF(r, g, b, a);
}

} // Quick
} // Qt3DRaytrace
