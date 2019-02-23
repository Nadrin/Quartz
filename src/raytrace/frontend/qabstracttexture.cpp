/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qabstracttexture_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QAbstractTexture::QAbstractTexture(QNode *parent)
    : QNode(*new QAbstractTexturePrivate, parent)
{}

QAbstractTexture::QAbstractTexture(QAbstractTexturePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{}

QTextureImage *QAbstractTexture::image() const
{
    Q_D(const QAbstractTexture);
    return d->m_image;
}

QTextureImageFactoryPtr QAbstractTexture::imageFactory() const
{
    Q_D(const QAbstractTexture);
    return d->m_imageFactory;
}

void QAbstractTexture::setImageFactory(const QTextureImageFactoryPtr &factory)
{
    Q_D(QAbstractTexture);
    d->m_imageFactory = factory;
    if(d->m_changeArbiter) {
        auto change = QPropertyUpdatedChangePtr::create(d->m_id);
        change->setPropertyName("imageFactory");
        change->setValue(QVariant::fromValue(d->m_imageFactory));
        d->notifyObservers(change);
    }
}

void QAbstractTexture::setImage(QTextureImage *image)
{
    Q_D(QAbstractTexture);
    if(d->m_image != image) {
        if(d->m_image) {
            d->unregisterDestructionHelper(d->m_image);
        }
        d->m_image = image;
        if(d->m_image) {
            if(!d->m_image->parent()) {
                d->m_image->setParent(this);
            }
            d->registerDestructionHelper(d->m_image, &QAbstractTexture::setImage, d->m_image);
        }
        emit imageChanged(d->m_image);
    }
}

void QAbstractTexture::sceneChangeEvent(const QSceneChangePtr &change)
{
    if(change->type() == PropertyUpdated) {
        auto propertyChange = qSharedPointerCast<QStaticPropertyUpdatedChangeBase>(change);
        if(propertyChange->propertyName() == QByteArrayLiteral("image")) {
            auto typedChange = qSharedPointerCast<QTextureImageChange>(change);
            setImage(typedChange->data.release());
        }
    }
}

QNodeCreatedChangeBasePtr QAbstractTexture::createNodeCreationChange() const
{
    Q_D(const QAbstractTexture);
    auto creationChange = QNodeCreatedChangePtr<QTextureData>::create(this);
    auto &data = creationChange->data;
    data.imageId = qIdForNode(d->m_image);
    data.imageFactory = d->m_imageFactory;
    return creationChange;
}

} // Qt3DRaytrace
