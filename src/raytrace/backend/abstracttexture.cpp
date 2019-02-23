/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <backend/abstracttexture_p.h>
#include <backend/managers_p.h>
#include <frontend/qabstracttexture_p.h>

#include <Qt3DCore/QPropertyUpdatedChange>
#include <QCoreApplication>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

AbstractTexture::AbstractTexture()
    : BackendNode(ReadWrite)
    , m_manager(nullptr)
{}

void AbstractTexture::setManager(TextureManager *manager)
{
    Q_ASSERT(manager);
    m_manager = manager;
}

void AbstractTexture::sceneChangeEvent(const QSceneChangePtr &change)
{
    if(change->type() == PropertyUpdated) {
        QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(change);
        const auto propertyName = propertyChange->propertyName();
        if(propertyName == QByteArrayLiteral("image")) {
            m_imageId = propertyChange->value().value<QNodeId>();
        }
        else if(propertyName == QByteArrayLiteral("imageFactory")) {
            m_imageFactory = propertyChange->value().value<QTextureImageFactoryPtr>();
            if(m_imageFactory && m_manager) {
                m_manager->markComponentDirty(peerId());
            }
        }
    }

    markDirty(AbstractRenderer::TextureDirty);
    BackendNode::sceneChangeEvent(change);
}

void AbstractTexture::loadImage()
{
    Q_ASSERT(m_imageFactory);

    std::unique_ptr<QTextureImage> image(m_imageFactory->create());
    if(image) {
        image->moveToThread(QCoreApplication::instance()->thread());

        auto change = QTextureImageChangePtr::create(peerId());
        change->setDeliveryFlags(QSceneChange::Nodes);
        change->setPropertyName("image");
        change->data = std::move(image);
        notifyObservers(change);
    }
}

void AbstractTexture::initializeFromPeer(const QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<QNodeCreatedChange<QTextureData>>(change);
    const auto &data = typedChange->data;

    m_imageId = data.imageId;
    m_imageFactory = data.imageFactory;
    if(m_imageFactory && m_manager) {
        m_manager->markComponentDirty(peerId());
    }

    markDirty(AbstractRenderer::TextureDirty);
}

} // Raytrace
} // Qt3DRaytrace
