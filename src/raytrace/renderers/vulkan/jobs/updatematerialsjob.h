/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <backend/handles_p.h>

#include <Qt3DCore/QAspectJob>
#include <QVector>

namespace Qt3DRaytrace {

namespace Raytrace {
class TextureManager;
} // Raytrace

namespace Vulkan {

class Renderer;

class UpdateMaterialsJob final : public Qt3DCore::QAspectJob
{
public:
    UpdateMaterialsJob(Renderer *renderer, Raytrace::TextureManager *textureManager);

    void setDirtyMaterialHandles(QVector<Raytrace::HMaterial> &materialHandles);
    void run() override;

private:
    Renderer *m_renderer;
    Raytrace::TextureManager *m_textureManager;
    QVector<Raytrace::HMaterial> m_dirtyMaterialHandles;
};

using UpdateMaterialsJobPtr = QSharedPointer<UpdateMaterialsJob>;

} // Vulkan
} // Qt3DRaytrace
