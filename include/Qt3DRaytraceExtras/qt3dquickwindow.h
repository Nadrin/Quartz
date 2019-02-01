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

namespace Qt3DCore {
class QEntity;
class QAbstractAspect;
namespace Quick {
class QQmlAspectEngine;
} // Quick
} // Qt3DCore

namespace Qt3DRaytraceExtras {
namespace Quick {

class Qt3DQuickWindowPrivate;

class QT3DRAYTRACEEXTRASSHARED_EXPORT Qt3DQuickWindow : public QWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Qt3DQuickWindow)
public:
    explicit Qt3DQuickWindow(QWindow *parent = nullptr);

    void registerAspect(Qt3DCore::QAbstractAspect *aspect);
    void registerAspect(const QString &name);

    void setSource(const QUrl &source);
    Qt3DCore::Quick::QQmlAspectEngine *engine() const;

protected:
    Qt3DQuickWindow(Qt3DQuickWindowPrivate &dd, QWindow *parent);

    bool event(QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void sceneCreated(QObject *root);
};

} // Quick
} // Qt3DRaytraceExtras
