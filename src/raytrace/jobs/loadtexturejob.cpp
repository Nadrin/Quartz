/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <jobs/loadtexturejob_p.h>
#include <backend/managers_p.h>
#include <backend/abstracttexture_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

LoadTextureJob::LoadTextureJob(NodeManagers *managers, const HAbstractTexture &handle)
    : m_nodeManagers(managers)
    , m_handle(handle)
{
    Q_ASSERT(m_nodeManagers);
}

void LoadTextureJob::run()
{
    AbstractTexture *texture = m_nodeManagers->textureManager.data(m_handle);
    if(texture) {
        texture->loadImage();
    }
}

} // Raytrace
} // Qt3DRaytrace
