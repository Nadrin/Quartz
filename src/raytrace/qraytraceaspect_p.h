/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qraytraceaspect.h>
#include <Qt3DCore/private/qabstractaspect_p.h>

#include <qt3draytrace_global_p.h>

namespace Qt3DRaytrace {

class QT3DRAYTRACESHARED_PRIVATE_EXPORT QRaytraceAspectPrivate : public Qt3DCore::QAbstractAspectPrivate
{
public:
    QRaytraceAspectPrivate();

    Q_DECLARE_PUBLIC(QRaytraceAspect)
};

} // Qt3DRaytrace
