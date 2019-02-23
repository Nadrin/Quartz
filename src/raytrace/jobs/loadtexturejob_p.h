/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <Qt3DCore/QAspectJob>

#include <backend/handles_p.h>

namespace Qt3DRaytrace {
namespace Raytrace {

struct NodeManagers;

class LoadTextureJob final : public Qt3DCore::QAspectJob
{
public:
    LoadTextureJob(NodeManagers *managers, const HAbstractTexture &handle);

    void run() override;

private:
    HAbstractTexture m_handle;
    NodeManagers *m_nodeManagers;
};

using LoadTextureJobPtr = QSharedPointer<LoadTextureJob>;

} // Raytrace
} // Qt3DRaytrace
