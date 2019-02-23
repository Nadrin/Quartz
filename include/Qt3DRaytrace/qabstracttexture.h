/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qt3draytrace_global.h>
#include <Qt3DRaytrace/qtextureimage.h>
#include <Qt3DRaytrace/qtextureimagefactory.h>

#include <Qt3DCore/QNode>

namespace Qt3DRaytrace {

class QAbstractTexturePrivate;

class QT3DRAYTRACESHARED_EXPORT QAbstractTexture : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRaytrace::QTextureImage* image READ image WRITE setImage NOTIFY imageChanged)
public:
    explicit QAbstractTexture(Qt3DCore::QNode *parent = nullptr);

    QTextureImage *image() const;

    QTextureImageFactoryPtr imageFactory() const;
    void setImageFactory(const QTextureImageFactoryPtr &factory);

public slots:
    void setImage(QTextureImage *image);

signals:
    void imageChanged(QTextureImage *image);

protected:
    explicit QAbstractTexture(QAbstractTexturePrivate &dd, Qt3DCore::QNode *parent = nullptr);
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

private:
    Q_DECLARE_PRIVATE(QAbstractTexture)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DRaytrace
