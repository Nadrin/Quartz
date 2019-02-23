/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>

#include <QSharedPointer>

namespace Qt3DRaytrace {

class QTextureImage;

class QT3DRAYTRACESHARED_EXPORT QTextureImageFactory
{
public:
    virtual ~QTextureImageFactory() = default;
    virtual QTextureImage *create() = 0;
};

using QTextureImageFactoryPtr = QSharedPointer<QTextureImageFactory>;

} // Qt3DRaytrace

Q_DECLARE_METATYPE(Qt3DRaytrace::QTextureImageFactoryPtr)
