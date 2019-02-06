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

class UpdateRenderParametersJob final : public Qt3DCore::QAspectJob
{
public:
    explicit UpdateRenderParametersJob(Renderer *renderer);

    void run() override;

private:
    Renderer *m_renderer;
};

using UpdateRenderParametersJobPtr = QSharedPointer<UpdateRenderParametersJob>;

} // Vulkan
} // Qt3DRaytrace
