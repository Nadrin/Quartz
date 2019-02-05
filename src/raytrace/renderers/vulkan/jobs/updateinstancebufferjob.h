/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>

#include <Qt3DCore/QAspectJob>

namespace Qt3DRaytrace {
namespace Vulkan {

class Renderer;

class UpdateInstanceBufferJob final : public Qt3DCore::QAspectJob
{
public:
    explicit UpdateInstanceBufferJob(Renderer *renderer);

    void run() override;

private:
    Renderer *m_renderer;
};

using UpdateInstanceBufferJobPtr = QSharedPointer<UpdateInstanceBufferJob>;

} // Vulkan
} // Qt3DRaytrace
