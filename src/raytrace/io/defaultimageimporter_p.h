/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <io/imageimporter_p.h>

namespace Qt3DRaytrace {
namespace Raytrace {

class DefaultImageImporter final : public ImageImporter
{
public:
    DefaultImageImporter();
    bool import(const QUrl &url, QImageData &data) override;
};

} // Raytrace
} // Qt3DRaytrace
