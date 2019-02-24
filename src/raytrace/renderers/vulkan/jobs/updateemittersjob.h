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

namespace Raytrace {
class TextureManager;
} // Raytrace

namespace Vulkan {

class Renderer;

class UpdateEmittersJob final : public Qt3DCore::QAspectJob
{
public:
    explicit UpdateEmittersJob(Renderer *renderer);

    void setTextureManager(Raytrace::TextureManager *textureManager);
    void run() override;

private:
    Renderer *m_renderer;
    Raytrace::TextureManager *m_textureManager;
};

using UpdateEmittersJobPtr = QSharedPointer<UpdateEmittersJob>;

} // Vulkan
} // Qt3DRaytrace
