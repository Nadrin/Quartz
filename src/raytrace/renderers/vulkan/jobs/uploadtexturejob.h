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

class UploadTextureJob final : public Qt3DCore::QAspectJob
{
public:
    UploadTextureJob(Renderer *renderer, const Raytrace::HTextureImage &handle);

    void run() override;

private:
    Renderer *m_renderer;
    Raytrace::HTextureImage m_handle;
};

using UploadTextureJobPtr = QSharedPointer<UploadTextureJob>;

} // Vulkan
} // Qt3DRaytrace
