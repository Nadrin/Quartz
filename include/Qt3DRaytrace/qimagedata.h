/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>
#include <QtCore/qmetatype.h>

#include <QByteArray>
#include <QSharedPointer>

namespace Qt3DRaytrace {

// TODO: Add support for layers and mipmaps.
struct QImageData
{
    enum class ValueType {
        Undefined = 0,
        UInt8     = 1,
        Float16   = 2,
        Float32   = 4,
    };
    enum class Format {
        Undefined = 0,
        RGB,
        BGR,
        RGBA,
        BGRA,
    };

    int width  = 0;
    int height = 0;
    int channels = 0;
    ValueType type = ValueType::Undefined;
    Format format = Format::Undefined;
    QByteArray data;
};

using QImageDataPtr = QSharedPointer<QImageData>;

} // Qt3DRaytrace

Q_DECLARE_METATYPE(Qt3DRaytrace::QImageData)
Q_DECLARE_METATYPE(Qt3DRaytrace::QImageDataPtr)
