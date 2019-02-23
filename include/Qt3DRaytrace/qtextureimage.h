/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>
#include <Qt3DRaytrace/qimagedata.h>

#include <Qt3DCore/QNode>

namespace Qt3DRaytrace {

class QTextureImagePrivate;

class QT3DRAYTRACESHARED_EXPORT QTextureImage : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    explicit QTextureImage(Qt3DCore::QNode *parent = nullptr);

    const QImageData &data() const;

    void setData(const QImageData &imageData);
    void clearData();

signals:
    void imageDataChanged(const QImageData &imageData);

protected:
    explicit QTextureImage(QTextureImagePrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QTextureImage)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DRaytrace
