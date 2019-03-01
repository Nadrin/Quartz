/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytraceExtras/qt3dquickwindow.h>
#include <QVulkanInstance>

class RenderWindow : public Qt3DRaytraceExtras::Quick::Qt3DQuickWindow
{
    Q_OBJECT
public:
    RenderWindow();

    static QVulkanInstance *createDefaultVulkanInstance();

    bool chooseSourceFile();
    bool setSourceFile(const QString &path);
    void setSceneName(const QString &name);

private:
    QString m_sceneName;
};
