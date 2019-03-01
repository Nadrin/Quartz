/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytraceExtras/qt3draytraceextras_global.h>

#include <QWindow>
#include <QString>
#include <QUrl>

class QVulkanInstance;
class QQmlEngine;

namespace Qt3DCore {
class QEntity;
class QAbstractAspect;
namespace Quick {
class QQmlAspectEngine;
} // Quick
} // Qt3DCore

namespace Qt3DInput {
class QInputAspect;
} // Qt3DInput

namespace Qt3DRaytrace {
class QRaytraceAspect;
} // Qt3DRaytrace

namespace Qt3DRaytraceExtras {
namespace Quick {

class Qt3DQuickWindowPrivate;

class QT3DRAYTRACEEXTRASSHARED_EXPORT Qt3DQuickWindow : public QWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Qt3DQuickWindow)
    Q_PROPERTY(CameraAspectRatioMode cameraAspectRatioMode READ cameraAspectRatioMode WRITE setCameraAspectRatioMode NOTIFY cameraAspectRatioModeChanged)
public:
    explicit Qt3DQuickWindow(QWindow *parent = nullptr);

    void registerAspect(Qt3DCore::QAbstractAspect *aspect);
    void registerAspect(const QString &name);

    Qt3DRaytrace::QRaytraceAspect *raytraceAspect() const;
    Qt3DInput::QInputAspect *inputAspect() const;

    QQmlEngine *qmlEngine() const;

    void setSource(const QUrl &source);
    Qt3DCore::Quick::QQmlAspectEngine *engine() const;

    enum CameraAspectRatioMode {
        AutomaticAspectRatio,
        UserAspectRatio,
    };
    Q_ENUM(CameraAspectRatioMode)

    CameraAspectRatioMode cameraAspectRatioMode() const;
    void setCameraAspectRatioMode(CameraAspectRatioMode mode);

signals:
    void cameraAspectRatioModeChanged(CameraAspectRatioMode mode);
    void aboutToClose();

protected:
    Qt3DQuickWindow(Qt3DQuickWindowPrivate &dd, QWindow *parent);

    virtual bool event(QEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

private slots:
    void sceneCreated(QObject *root);
    void updateCameraAspectRatio();
};

} // Quick
} // Qt3DRaytraceExtras
