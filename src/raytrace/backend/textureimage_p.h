/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <backend/backendnode_p.h>

#include <Qt3DRaytrace/qimagedata.h>

namespace Qt3DRaytrace {
namespace Raytrace {

class TextureImageManager;

class TextureImage : public BackendNode
{
public:
    const QImageData &data() const { return m_data; }

    void setManager(TextureImageManager *manager);
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;

    TextureImageManager *m_manager = nullptr;
    QImageData m_data;
};

class TextureImageNodeMapper final : public BackendNodeMapper<TextureImage, TextureImageManager>
{
public:
    TextureImageNodeMapper(TextureImageManager *manager, AbstractRenderer *renderer)
        : BackendNodeMapper(manager, renderer)
    {}

    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const override
    {
        auto textureImage = static_cast<TextureImage*>(BackendNodeMapper::create(change));
        textureImage->setManager(m_manager);
        return textureImage;
    }
};

} // Raytrace
} // Qt3DRaytrace
