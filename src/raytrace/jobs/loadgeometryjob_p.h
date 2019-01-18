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

class LoadGeometryJob final : public Qt3DCore::QAspectJob
{
public:
    LoadGeometryJob(NodeManagers *managers, const HGeometryRenderer &handle);

    void run() override;

private:
    HGeometryRenderer m_handle;
    NodeManagers *m_nodeManagers;
};

using LoadGeometryJobPtr = QSharedPointer<LoadGeometryJob>;

} // Raytrace
} // Qt3DRaytrace
