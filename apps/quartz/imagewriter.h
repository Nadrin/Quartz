/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <QString>
#include <QThread>

#include <Qt3DRaytrace/qimagedata.h>

class ImageWriter : public QThread
{
    Q_OBJECT
public:
    explicit ImageWriter(QObject *parent);

    void setOutputPath(const QString &path);
    void setImage(const Qt3DRaytrace::QImageDataPtr &image);
    void setQuality(int quality);

private:
    void run() override;

    QString m_outputPath;
    Qt3DRaytrace::QImageDataPtr m_image;
    int m_quality;
};
