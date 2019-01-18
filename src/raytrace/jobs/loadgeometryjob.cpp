/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <jobs/loadgeometryjob_p.h>
#include <backend/managers_p.h>
#include <backend/geometryrenderer_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

LoadGeometryJob::LoadGeometryJob(NodeManagers *managers, const HGeometryRenderer &handle)
    : m_nodeManagers(managers)
    , m_handle(handle)
{
    Q_ASSERT(m_nodeManagers);
}

void LoadGeometryJob::run()
{
    GeometryRenderer *geometryRenderer = m_nodeManagers->geometryRendererManager.data(m_handle);
    if(geometryRenderer) {
        geometryRenderer->loadGeometry();
    }
}

} // Raytrace
} // Qt3DRaytrace
