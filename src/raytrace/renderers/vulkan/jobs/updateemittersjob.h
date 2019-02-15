/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <backend/handles_p.h>

#include <Qt3DCore/QAspectJob>

namespace Qt3DRaytrace {
namespace Vulkan {

class Renderer;

class UpdateEmittersJob final : public Qt3DCore::QAspectJob
{
public:
    explicit UpdateEmittersJob(Renderer *renderer);

    void run() override;

private:
    Renderer *m_renderer;
};

using UpdateEmittersJobPtr = QSharedPointer<UpdateEmittersJob>;

} // Vulkan
} // Qt3DRaytrace
