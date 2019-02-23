/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <backend/textureimage_p.h>
#include <backend/managers_p.h>
#include <frontend/qtextureimage_p.h>

#include <Qt3DCore/QPropertyUpdatedChange>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

void TextureImage::setManager(TextureImageManager *manager)
{
    Q_ASSERT(manager);
    m_manager = manager;
}

void TextureImage::sceneChangeEvent(const QSceneChangePtr &change)
{
    if(change->type() == PropertyUpdated) {
        QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(change);
        if(propertyChange->propertyName() == QByteArrayLiteral("data")) {
            m_data = propertyChange->value().value<QImageData>();
            if(m_manager) {
                m_manager->markComponentDirty(peerId());
            }
            markDirty(AbstractRenderer::TextureDirty);
        }
    }
    BackendNode::sceneChangeEvent(change);
}

void TextureImage::initializeFromPeer(const QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QImageData>>(change);
    m_data = typedChange->data;

    if(m_manager) {
        m_manager->markComponentDirty(peerId());
    }
    markDirty(AbstractRenderer::TextureDirty);
}

} // Raytrace
} // Qt3DRaytrace
