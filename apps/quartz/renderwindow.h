/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytraceExtras/qt3dquickwindow.h>
#include <Qt3DRaytrace/qimagedata.h>
#include <Qt3DRaytrace/qrenderimage.h>

#include <QVulkanInstance>

class RenderWindow : public Qt3DRaytraceExtras::Quick::Qt3DQuickWindow
{
    Q_OBJECT
public:
    RenderWindow();

    static QVulkanInstance *createDefaultVulkanInstance();

    bool requestSourceFile();
    bool setSourceFile(const QString &path);
    void setSceneName(const QString &name);

    bool requestSaveImage();
    bool saveImage(const QString &path);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateTitle();
    void imageReady(Qt3DRaytrace::QRenderImage type, Qt3DRaytrace::QImageDataPtr image);

private:
    QString m_sceneName;
    QString m_saveImageRequestPath;
};
