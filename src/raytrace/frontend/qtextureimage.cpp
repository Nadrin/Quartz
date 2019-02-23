/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <frontend/qtextureimage_p.h>

using namespace Qt3DCore;

namespace Qt3DRaytrace {

QTextureImage::QTextureImage(QNode *parent)
    : QNode(*new QTextureImagePrivate, parent)
{}

const QImageData &QTextureImage::data() const
{
    Q_D(const QTextureImage);
    return d->m_data;
}

void QTextureImage::setData(const QImageData &imageData)
{
    Q_D(QTextureImage);
    d->m_data = imageData;
    QNodePrivate::get(this)->notifyPropertyChange("data", QVariant::fromValue(d->m_data));
    emit imageDataChanged(d->m_data);
}

void QTextureImage::clearData()
{
    setData(QImageData());
}

QTextureImage::QTextureImage(QTextureImagePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{}

QNodeCreatedChangeBasePtr QTextureImage::createNodeCreationChange() const
{
    Q_D(const QTextureImage);
    auto creationChange = QNodeCreatedChangePtr<QImageData>::create(this);
    creationChange->data = d->m_data;
    return creationChange;
}

} // Qt3DRaytrace
