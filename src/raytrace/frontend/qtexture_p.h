/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qtexture.h>
#include <Qt3DRaytrace/qtextureimagefactory.h>
#include <frontend/qabstracttexture_p.h>
#include <io/imageimporter_p.h>

#include <QScopedPointer>

namespace Qt3DRaytrace {

class QTexturePrivate : public QAbstractTexturePrivate
{
public:
    Q_DECLARE_PUBLIC(QTexture)

    QUrl m_source;
    QTexture::Status m_status = QTexture::None;
};

class TextureImageLoader final : public QTextureImageFactory
{
public:
    explicit TextureImageLoader(const QTexture *texture);

    QTextureImage *create() override;

private:
    QScopedPointer<Raytrace::ImageImporter> m_importer;
    QUrl m_source;
};

} // Qt3DRaytrace
