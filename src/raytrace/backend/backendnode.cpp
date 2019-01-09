/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <backend/backendnode_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

BackendNode::BackendNode(QBackendNode::Mode mode)
    : QBackendNode(mode)
{}

} // Raytrace
} // Qt3DRaytrace
