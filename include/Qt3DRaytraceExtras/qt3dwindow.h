/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytraceExtras/qt3draytraceextras_global.h>

#include <QWindow>
#include <QString>

class QVulkanInstance;

namespace Qt3DCore {
class QEntity;
class QAbstractAspect;
} // Qt3DCore

namespace Qt3DRaytraceExtras {

class Qt3DWindowPrivate;

class QT3DRAYTRACEEXTRASSHARED_EXPORT Qt3DWindow : public QWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Qt3DWindow)
public:
    explicit Qt3DWindow(QWindow *parent = nullptr);

    void registerAspect(Qt3DCore::QAbstractAspect *aspect);
    void registerAspect(const QString &name);

    void setRootEntity(Qt3DCore::QEntity *root);

protected:
    Qt3DWindow(Qt3DWindowPrivate &dd, QWindow *parent);

    bool event(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
};

} // Qt3DRaytraceExtras
