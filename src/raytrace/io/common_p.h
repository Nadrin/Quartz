/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <QUrl>

namespace Qt3DRaytrace {
namespace Raytrace {

static inline QString getAssetPathFromUrl(const QUrl &url)
{
    if(url.isLocalFile()) {
        return url.toLocalFile();
    }
    else if(url.scheme() == QStringLiteral("qrc")) {
        return QStringLiteral(":") + url.path();
    }
    else {
        return url.toString();
    }
}

} // Raytrace
} // Qt3DRaytrace
