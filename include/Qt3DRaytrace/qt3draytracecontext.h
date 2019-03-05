/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>

#include <QObject>
#include <QColor>

namespace Qt3DRaytrace {
namespace Quick {

class QT3DRAYTRACESHARED_EXPORT Qt3DRaytraceContext : public QObject
{
    Q_OBJECT
public:
    explicit Qt3DRaytraceContext(QObject *parent = nullptr);

    Q_INVOKABLE QColor lrgba(qreal r, qreal g, qreal b, qreal a=1.0) const;
    Q_INVOKABLE QColor srgba(qreal r, qreal g, qreal b, qreal a=1.0) const;
};

} // Quick
} // Qt3DRaytrace
