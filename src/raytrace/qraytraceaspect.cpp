/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <Qt3DRaytrace/qraytraceaspect.h>
#include <qraytraceaspect_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QRaytraceAspect::QRaytraceAspect(QObject *parent)
    : QRaytraceAspect(*new QRaytraceAspectPrivate, parent)
{}

QRaytraceAspect::~QRaytraceAspect()
{

}

QRaytraceAspect::QRaytraceAspect(QRaytraceAspectPrivate &dd, QObject *parent)
    : QAbstractAspect(dd, parent)
{
    setObjectName(QStringLiteral("Raytrace Aspect"));
}

QRaytraceAspectPrivate::QRaytraceAspectPrivate()
{

}

} // Qt3DRaytrace
