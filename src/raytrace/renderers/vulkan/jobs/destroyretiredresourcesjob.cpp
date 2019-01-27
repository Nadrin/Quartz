/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/jobs/destroyretiredresourcesjob.h>
#include <renderers/vulkan/renderer.h>
#include <renderers/vulkan/managers/commandbuffermanager.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Vulkan {

DestroyRetiredResourcesJob::DestroyRetiredResourcesJob(Renderer *renderer)
    : m_renderer(renderer)
{
    Q_ASSERT(m_renderer);
}

void DestroyRetiredResourcesJob::run()
{
    auto *commandBufferManager = m_renderer->commandBufferManager();
    if(commandBufferManager) {
        commandBufferManager->destroyRetiredResources();
    }
    m_renderer->destroyRetiredResources();
}

} // Vulkan
} // Qt3DRaytrace
