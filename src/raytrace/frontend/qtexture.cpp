/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qtexture_p.h>
#include <io/defaultimageimporter_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

// TODO: Implement status change.
// TODO: Make texture image loader configurable.

QTexture::QTexture(QNode *parent)
    : QAbstractTexture(*new QTexturePrivate, parent)
{}

QTexture::QTexture(QTexturePrivate &dd, QNode *parent)
    : QAbstractTexture(dd, parent)
{}

QUrl QTexture::source() const
{
    Q_D(const QTexture);
    return d->m_source;
}

QTexture::Status QTexture::status() const
{
    Q_D(const QTexture);
    return d->m_status;
}

void QTexture::setSource(const QUrl &source)
{
    Q_D(QTexture);
    if(d->m_source != source) {
        d->m_source = source;
        setImageFactory(QTextureImageFactoryPtr(new TextureImageLoader(this)));
        emit sourceChanged(source);
    }
}

TextureImageLoader::TextureImageLoader(const QTexture *texture)
    : m_importer(new Raytrace::DefaultImageImporter)
    , m_source(texture->source())
{}

QTextureImage *TextureImageLoader::create()
{
    Q_ASSERT(m_importer);

    if(m_source.isEmpty()) {
        qCWarning(logImport) << "Texture image source path is empty";
        return nullptr;
    }

    QImageData imageData;
    if(m_importer->import(m_source, imageData)) {
        QTextureImage *image = new QTextureImage;
        image->setData(imageData);
        return image;
    }
    else {
        return nullptr;
    }
}

} // Qt3DRaytrace
