/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>

class QObject;
class QSurface;

namespace Qt3DRaytrace {

class QT3DRAYTRACESHARED_EXPORT QRendererInterface
{
public:
    virtual ~QRendererInterface() = default;

    enum class API {
        Vulkan,
        D3D12,
    };
    virtual API api() const = 0;

    virtual QSurface *surface() const = 0;
    virtual void setSurface(QObject *surfaceObject) = 0;

    virtual void preInitResources() {}
    virtual void initResources() {}
    virtual void initSwapChainResources() {}
    virtual void releaseSwapChainResources() {}
    virtual void releaseResources() {}
    virtual void startNextFrame() {}
};

} // Qt3DRaytrace
