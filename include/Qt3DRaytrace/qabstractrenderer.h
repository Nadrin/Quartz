/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>

class QSurface;

namespace Qt3DCore {
class QAbstractFrameAdvanceService;
} // Qt3DCore

namespace Qt3DRaytrace {

class QT3DRAYTRACESHARED_EXPORT QAbstractRenderer
{
public:
    virtual ~QAbstractRenderer() = default;

    enum class API {
        Vulkan,
        D3D12,
    };
    virtual API api() const = 0;

    virtual QSurface *surface() const = 0;
    virtual Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const = 0;
};

} // Qt3DRaytrace
