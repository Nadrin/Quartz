/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytraceExtras/qt3dquickwindow.h>
#include <QtGui/private/qwindow_p.h>

#include <Qt3DQuick/QQmlAspectEngine>
#include <Qt3DInput/QInputAspect>
#include <Qt3DLogic/QLogicAspect>
#include <Qt3DInput/QInputSettings>

#include <Qt3DRaytrace/qraytraceaspect.h>
#include <Qt3DRaytrace/qcamera.h>

#include <QPointer>
#include <QScopedPointer>

namespace Qt3DRaytraceExtras {
namespace Quick {

class Qt3DQuickWindowPrivate : public QWindowPrivate
{
    Q_DECLARE_PUBLIC(Qt3DQuickWindow)
public:
    Qt3DQuickWindowPrivate();

    QScopedPointer<Qt3DCore::Quick::QQmlAspectEngine> m_engine;

    Qt3DRaytrace::QRaytraceAspect *m_raytraceAspect;
    Qt3DInput::QInputAspect *m_inputAspect;
    Qt3DLogic::QLogicAspect *m_logicAspect;

    QPointer<Qt3DRaytrace::QCamera> m_camera;
    Qt3DQuickWindow::CameraAspectRatioMode m_cameraAspectRatioMode;

    QUrl m_source;
    bool m_initialized;
};

} // Quick
} // Qt3DRaytraceExtras
