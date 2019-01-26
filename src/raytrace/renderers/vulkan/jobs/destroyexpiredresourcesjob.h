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

class DestroyExpiredResourcesJob final : public Qt3DCore::QAspectJob
{
public:
    explicit DestroyExpiredResourcesJob(Renderer *renderer);

    void run() override;

private:
    Renderer *m_renderer;
};

using DestroyExpiredResourcesJobPtr = QSharedPointer<DestroyExpiredResourcesJob>;

} // Vulkan
} // Qt3DRaytrace
