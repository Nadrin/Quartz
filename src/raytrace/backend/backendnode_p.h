/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <Qt3DCore/QBackendNode>

namespace Qt3DRaytrace {

class QAbstractRenderer;

namespace Raytrace {

class BackendNode : public Qt3DCore::QBackendNode
{
public:
    explicit BackendNode(Qt3DCore::QBackendNode::Mode mode = ReadOnly);

    void setRenderer(QAbstractRenderer *renderer)
    {
        m_renderer = renderer;
    }

protected:
    QAbstractRenderer *m_renderer = nullptr;
};

} // Raytrace
} // Qt3DRaytrace
