/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <Qt3DRaytrace/qrendererinterface.h>

class QSurface;

namespace Qt3DCore {
class QAbstractFrameAdvanceService;
} // Qt3DCore

namespace Qt3DRaytrace {

class AbstractRenderer : public QRendererInterface
{
public:
    virtual ~AbstractRenderer() = default;
    virtual Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const = 0;
};

} // Qt3DRaytrace
