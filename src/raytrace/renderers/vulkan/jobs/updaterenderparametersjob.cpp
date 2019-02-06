/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/jobs/updaterenderparametersjob.h>
#include <renderers/vulkan/renderer.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Vulkan {

UpdateRenderParametersJob::UpdateRenderParametersJob(Renderer *renderer)
    : m_renderer(renderer)
{
    Q_ASSERT(m_renderer);
}

void UpdateRenderParametersJob::run()
{
    auto *cameraManager = m_renderer->cameraManager();
    if(cameraManager) {
        cameraManager->updateParameters();
    }
}

} // Vulkan
} // Qt3DRaytrace
