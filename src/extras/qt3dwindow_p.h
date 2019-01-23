/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytraceExtras/qt3dwindow.h>
#include <QtGui/private/qwindow_p.h>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DInput/QInputAspect>
#include <Qt3DLogic/QLogicAspect>
#include <Qt3DInput/QInputSettings>

#include <Qt3DRaytrace/qraytraceaspect.h>

#include <QScopedPointer>

namespace Qt3DRaytraceExtras {

class Qt3DWindowPrivate : public QWindowPrivate
{
    Q_DECLARE_PUBLIC(Qt3DWindow)
public:
    Qt3DWindowPrivate();
    ~Qt3DWindowPrivate();

    QScopedPointer<Qt3DCore::QAspectEngine> m_aspectEngine;

    Qt3DRaytrace::QRaytraceAspect *m_raytraceAspect;
    Qt3DInput::QInputAspect *m_inputAspect;
    Qt3DLogic::QLogicAspect *m_logicAspect;

    Qt3DInput::QInputSettings *m_inputSettings;
    Qt3DCore::QEntity *m_root;
    Qt3DCore::QEntity *m_userRoot;

    bool m_initialized = false;
};

} // Qt3DRaytraceExtras
