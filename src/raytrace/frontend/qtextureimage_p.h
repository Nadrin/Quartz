/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qtextureimage.h>
#include <Qt3DRaytrace/qimagedata.h>
#include <Qt3DCore/private/qnode_p.h>

namespace Qt3DRaytrace {

class QTextureImagePrivate : public Qt3DCore::QNodePrivate
{
public:
    Q_DECLARE_PUBLIC(QTextureImage)
    QImageData m_data;
};

} // Qt3DRaytrace
