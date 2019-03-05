/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>

#include <QColor>
#include <cmath>

namespace Qt3DRaytrace {

static inline QColor to_sRgb(const QColor &c)
{
    auto sRgb = [](qreal u) -> qreal {
        if(u < 0.0031308) return u * 12.92;
        else return std::pow(u * 1.055, 1.0 / 2.4) - 0.055;
    };
    return QColor::fromRgbF(sRgb(c.redF()), sRgb(c.greenF()), sRgb(c.blueF()), c.alphaF());
}

static inline QColor to_linearRgb(const QColor &c)
{
    auto inv_sRgb = [](qreal u) -> qreal {
        if(u < 0.04045) return u / 12.92;
        else return std::pow(((u + 0.055) / 1.055), 2.4);
    };
    return QColor::fromRgbF(inv_sRgb(c.redF()), inv_sRgb(c.greenF()), inv_sRgb(c.blueF()), c.alphaF());
}

} // Qt3DRaytrace
