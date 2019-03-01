/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include "imagewriter.h"

#include <utility>
#include <stb_image_write.h>

#include <QFileInfo>
#include <QTextStream>

template<typename T>
static void swapChannelsRedBlue(QByteArray &imageData, int channels)
{
    Q_ASSERT(channels >= 3);
    T *values    = reinterpret_cast<T*>(imageData.data());
    T *valuesEnd = reinterpret_cast<T*>(imageData.data() + imageData.size());
    for(; values < valuesEnd; values += channels) {
        std::swap(values[0], values[2]);
    }
}

ImageWriter::ImageWriter(QObject *parent)
    : QThread(parent)
    , m_quality(100)
{
    QObject::connect(this, &ImageWriter::finished, this, &ImageWriter::deleteLater);
}

void ImageWriter::setOutputPath(const QString &path)
{
    m_outputPath = path;
}

void ImageWriter::setImage(const Qt3DRaytrace::QImageDataPtr &image)
{
    m_image = image;
}

void ImageWriter::setQuality(int quality)
{
    m_quality = quality;
}

void ImageWriter::run()
{
    Q_ASSERT(m_outputPath.size() > 0);
    Q_ASSERT(!m_image.isNull());

    using Format = Qt3DRaytrace::QImageData::Format;
    using ValueType = Qt3DRaytrace::QImageData::ValueType;

    if(m_image->format == Format::BGR || m_image->format == Format::BGRA) {
        if(m_image->type == ValueType::UInt8) {
            swapChannelsRedBlue<quint8>(m_image->data, m_image->channels);
        }
        else if(m_image->type == ValueType::Float32) {
            swapChannelsRedBlue<float>(m_image->data, m_image->channels);
        }
        else {
            Q_ASSERT_X(0, Q_FUNC_INFO, "Unsupported image value type");
        }
    }

    int result = 0;
    int stride = m_image->width * m_image->channels * static_cast<int>(m_image->type);

    QString fileFormat = QFileInfo(m_outputPath).suffix().toLower();
    if(fileFormat == QStringLiteral("jpg")) {
        result = stbi_write_jpg(m_outputPath.toUtf8().constData(),
                                m_image->width, m_image->height, m_image->channels,
                                m_image->data.constData(), m_quality);
    }
    else if(fileFormat == QStringLiteral("png")) {
        result = stbi_write_png(m_outputPath.toUtf8().constData(),
                                m_image->width, m_image->height, m_image->channels,
                                m_image->data.constData(), stride);
    }
    else if(fileFormat == QStringLiteral("bmp")) {
        result = stbi_write_bmp(m_outputPath.toUtf8().constData(),
                                m_image->width, m_image->height, m_image->channels,
                                m_image->data.constData());
    }
    else if(fileFormat == QStringLiteral("tga")) {
        result = stbi_write_tga(m_outputPath.toUtf8().constData(),
                                m_image->width, m_image->height, m_image->channels,
                                m_image->data.constData());
    }
    else if(fileFormat == QStringLiteral("hdr")) {
        result = stbi_write_hdr(m_outputPath.toUtf8().constData(),
                                m_image->width, m_image->height, m_image->channels,
                                reinterpret_cast<const float*>(m_image->data.constData()));
    }
    else {
        Q_ASSERT_X(0, Q_FUNC_INFO, "Unsupported file format");
    }

    if(result) {
        QTextStream(stdout) << "Image saved: " << m_outputPath << '\n';
    }
    else {
        QTextStream(stderr) << "Error: Failed to write image file: " << m_outputPath << '\n';
    }
}
