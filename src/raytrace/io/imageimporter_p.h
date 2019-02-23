/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>

#include <Qt3DRaytrace/qimagedata.h>
#include <QUrl>

namespace Qt3DRaytrace {
namespace Raytrace {

class ImageImporter
{
public:
    virtual ~ImageImporter() = default;
    virtual bool import(const QUrl &url, QImageData &data) = 0;
};

} // Raytrace
} // Qt3DRaytrace
