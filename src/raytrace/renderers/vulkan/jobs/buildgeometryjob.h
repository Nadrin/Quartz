/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <Qt3DCore/QAspectJob>

#include <backend/handles_p.h>

namespace Qt3DRaytrace {
namespace Vulkan {

class Renderer;

class BuildGeometryJob final : public Qt3DCore::QAspectJob
{
public:
    BuildGeometryJob(Renderer *renderer, const Raytrace::HGeometry &handle);

    void run() override;

private:
    Renderer *m_renderer;
    Raytrace::HGeometry m_handle;
};

using BuildGeometryJobPtr = QSharedPointer<BuildGeometryJob>;

} // Vulkan
} // Qt3DRaytrace
