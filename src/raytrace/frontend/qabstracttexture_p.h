/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qabstracttexture.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qtypedpropertyupdatechange_p.h>

namespace Qt3DRaytrace {

class QAbstractTexturePrivate : public Qt3DCore::QNodePrivate
{
public:
    Q_DECLARE_PUBLIC(QAbstractTexture)
    QTextureImage *m_image = nullptr;
    QTextureImageFactoryPtr m_imageFactory;
};

struct QTextureData
{
    Qt3DCore::QNodeId imageId;
    QTextureImageFactoryPtr imageFactory;
};

class QTextureImage;

// TODO: Is std::unique_ptr really needed here?
using QTextureImageChange = Qt3DCore::QTypedPropertyUpdatedChange<std::unique_ptr<QTextureImage>>;
using QTextureImageChangePtr = Qt3DCore::QTypedPropertyUpdatedChangePtr<std::unique_ptr<QTextureImage>>;

} // Qt3DRaytrace
