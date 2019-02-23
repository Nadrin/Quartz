/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>
#include <QtCore/qmetatype.h>

#include <QByteArray>

namespace Qt3DRaytrace {

enum class QImageDataType
{
    Undefined = 0,
    UInt8     = 1,
    Float16   = 2,
    Float32   = 4,
};

// TODO: Add support for layers and mipmaps.
struct QImageData
{
    int imageWidth  = 0;
    int imageHeight = 0;
    int numChannels = 0;
    QImageDataType type = QImageDataType::Undefined;
    QByteArray data;
};

} // Qt3DRaytrace

Q_DECLARE_METATYPE(Qt3DRaytrace::QImageData)
