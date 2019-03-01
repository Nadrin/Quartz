/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include "renderwindow.h"
#include "imagewriter.h"

#include <Qt3DRaytrace/qraytraceaspect.h>

#include <QScopedPointer>

#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QKeyEvent>

#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include <QRegularExpression>

namespace Config {
static constexpr int     UpdateTitleInterval = 200;
static constexpr int     SaveImageQuality = 100;
static constexpr Qt::Key SaveImageKey = Qt::Key_F2;
} // Config

RenderWindow::RenderWindow()
{
    QTimer *updateTitleTimer = new QTimer(this);
    QObject::connect(updateTitleTimer, &QTimer::timeout, this, &RenderWindow::updateTitle);
    updateTitleTimer->start(Config::UpdateTitleInterval);

    QObject::connect(raytraceAspect(), &Qt3DRaytrace::QRaytraceAspect::imageReady, this, &RenderWindow::imageReady);
}

QVulkanInstance *RenderWindow::createDefaultVulkanInstance()
{
    QScopedPointer<QVulkanInstance> vkInstance(new QVulkanInstance);
    vkInstance->setApiVersion(QVersionNumber(1, 1));
#ifdef QUARTZ_DEBUG
    vkInstance->setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");
#endif
    if(!vkInstance->create()) {
        QMessageBox::critical(nullptr, tr("Quartz"), QString("%1: %2")
                              .arg(tr("Failed to create Vulkan instance"))
                              .arg(vkInstance->errorCode()));
        return nullptr;
    }
    return vkInstance.take();
}

bool RenderWindow::requestSourceFile()
{
    QFileDialog dialog(nullptr, tr("Open QML file"));
    dialog.setNameFilter(tr("QML Files (*.qml)"));
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setDirectory(QDir::currentPath());
    if(dialog.exec()) {
        const QString path = dialog.selectedFiles().at(0);
        return setSourceFile(path);
    }
    return false;
}

bool RenderWindow::setSourceFile(const QString &path)
{
    const QString absolutePath = QDir::current().absoluteFilePath(path);
    {
        QFile sceneFile(absolutePath);
        if(!sceneFile.open(QFile::ReadOnly)) {
            QMessageBox::critical(nullptr, tr("Quartz"), QString("%1: %2")
                                  .arg(tr("Cannot open input file"))
                                  .arg(absolutePath));
            return false;
        }
    }

    QDir::setCurrent(QFileInfo(absolutePath).absolutePath());

    setSource(QUrl::fromLocalFile(absolutePath));
    setSceneName(QFileInfo(absolutePath).fileName());
    return true;
}

void RenderWindow::setSceneName(const QString &name)
{
    m_sceneName = name;
    updateTitle();
}

bool RenderWindow::requestSaveImage()
{
    QFileDialog dialog(nullptr, tr("Save Image"));
    dialog.setNameFilter(tr("Image files (*.jpg *.png *.bmp *.tga *.hdr)"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(QDir::currentPath());
    if(dialog.exec()) {
        const QString path = dialog.selectedFiles().at(0);
        return saveImage(path);
    }
    return false;
}

bool RenderWindow::saveImage(const QString &path)
{
    static const QRegularExpression ValidImageFormats("jpg|png|bmp|tga|hdr");

    const QString fileFormat = QFileInfo(path).suffix().toLower();
    if(!ValidImageFormats.match(fileFormat).hasMatch()) {
        QMessageBox::critical(nullptr, tr("Quartz"), tr("Unsupported image format"));
        return false;
    }

    if(fileFormat == QStringLiteral("hdr")) {
        raytraceAspect()->requestImage(Qt3DRaytrace::QRenderImage::HDR);
    }
    else {
        raytraceAspect()->requestImage(Qt3DRaytrace::QRenderImage::FinalLDR);
    }

    m_saveImageRequestPath = path;
    return true;
}

void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Config::SaveImageKey) {
        requestSaveImage();
    }

    QWindow::keyPressEvent(event);
}

void RenderWindow::updateTitle()
{
    Qt3DRaytrace::QRenderStatistics statistics;
    if(raytraceAspect()->queryRenderStatistics(statistics)) {
        double frameTime = std::max(statistics.cpuFrameTime, statistics.gpuFrameTime);
        if(qFuzzyIsNull(frameTime)) {
            return;
        }

        QString statisticsString = QString("CPU: %1 ms | GPU: %2 ms | FPS: %3 | Current image: %5 s / %6 spp")
                .arg(statistics.cpuFrameTime, 0, 'f', 2)
                .arg(statistics.gpuFrameTime, 0, 'f', 2)
                .arg(1000.0 / frameTime, 0, 'f', 0)
                .arg(statistics.totalRenderTime, 0, 'f', 2)
                .arg(statistics.numFramesRendered);

        setTitle(QString("%1 - %2 [ %3 ]")
                 .arg(m_sceneName)
                 .arg(QApplication::applicationName())
                 .arg(statisticsString));
    }
    else {
        setTitle(QString("%1 - %2")
                 .arg(m_sceneName)
                 .arg(QApplication::applicationName()));
    }
}

void RenderWindow::imageReady(Qt3DRaytrace::QRenderImage type, Qt3DRaytrace::QImageDataPtr image)
{
    Q_UNUSED(type);
    Q_ASSERT(m_saveImageRequestPath.length() > 0);

    ImageWriter *imageWriter = new ImageWriter(this);
    imageWriter->setImage(image);
    imageWriter->setOutputPath(m_saveImageRequestPath);
    imageWriter->setQuality(Config::SaveImageQuality);
    imageWriter->start();

    m_saveImageRequestPath.clear();
}
