/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>

#include <QColor>

namespace Qt3DRaytrace {
namespace Raytrace {

struct LinearColor
{
    LinearColor()
        : r(0.0f), g(0.0f), b(0.0f)
    {}
    LinearColor(float r, float g, float b)
        : r(r), g(g), b(b)
    {}
    LinearColor(const QColor &c)
        : r(float(c.redF()))
        , g(float(c.greenF()))
        , b(float(c.blueF()))
    {}

    void writeToBuffer(float *buffer) const
    {
        buffer[0] = r;
        buffer[1] = g;
        buffer[2] = b;
    }

    float r, g, b;
};

} // Raytrace
} // Qt3DRaytrace
