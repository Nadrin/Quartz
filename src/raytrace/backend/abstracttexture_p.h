/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <backend/backendnode_p.h>
#include <Qt3DRaytrace/qtextureimagefactory.h>

namespace Qt3DRaytrace {
namespace Raytrace {

class TextureManager;

class AbstractTexture : public BackendNode
{
public:
    AbstractTexture();

    void setManager(TextureManager *manager);
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;
    void loadImage();

    Qt3DCore::QNodeId imageId() const { return m_imageId; }
    QTextureImageFactoryPtr imageFactory() const { return m_imageFactory; }

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;

    TextureManager *m_manager;
    Qt3DCore::QNodeId m_imageId;
    QTextureImageFactoryPtr m_imageFactory;
};

class TextureNodeMapper final : public BackendNodeMapper<AbstractTexture, TextureManager>
{
public:
    TextureNodeMapper(TextureManager *manager, AbstractRenderer *renderer)
        : BackendNodeMapper(manager, renderer)
    {}

    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const override
    {
        auto texture = static_cast<AbstractTexture*>(BackendNodeMapper::create(change));
        texture->setManager(m_manager);
        return texture;
    }
};

} // Raytrace
} // Qt3DRaytrace
