/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytraceExtras/qt3draytraceextras_global.h>

#include <QVulkanWindow>
#include <QScopedPointer>

#include <QAspectEngine>
#include <QInputAspect>
#include <QLogicAspect>
#include <QInputSettings>

#include <Qt3DRaytrace/qraytraceaspect.h>

namespace Qt3DRaytraceExtras {

class QT3DRAYTRACEEXTRASSHARED_EXPORT Qt3DWindow : public QVulkanWindow
{
    Q_OBJECT
public:
    Qt3DWindow();

    QVulkanWindowRenderer *createRenderer() override;

    void setRootEntity(Qt3DCore::QEntity *root);

protected:
    void exposeEvent(QExposeEvent *event) override;

    QScopedPointer<Qt3DCore::QAspectEngine> m_aspectEngine;

    Qt3DRaytrace::QRaytraceAspect *m_raytraceAspect;
    Qt3DInput::QInputAspect *m_inputAspect;
    Qt3DLogic::QLogicAspect *m_logicAspect;

    QScopedPointer<Qt3DInput::QInputSettings> m_inputSettings;
    Qt3DCore::QEntity *m_root;
    Qt3DCore::QEntity *m_userRoot;

    bool m_initialized = false;
};

} // Qt3DRaytraceExtras
