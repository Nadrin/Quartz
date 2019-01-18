/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>

#include <QVector2D>
#include <QVector3D>

#include <QVector>

namespace Qt3DRaytrace {

struct QVertex
{
    QVector3D position;
    QVector3D normal;
    QVector3D tangent;
    QVector3D bitangent;
    QVector2D texcoord;
};

struct QTriangle
{
    quint32 vertices[3];
};

struct QGeometryData
{
    QVector<QVertex> vertices;
    QVector<QTriangle> faces;
};

} // Qt3DRaytrace

Q_DECLARE_METATYPE(Qt3DRaytrace::QVertex)
Q_DECLARE_METATYPE(Qt3DRaytrace::QTriangle)
Q_DECLARE_METATYPE(Qt3DRaytrace::QGeometryData)
