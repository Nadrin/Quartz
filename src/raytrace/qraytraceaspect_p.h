/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <QScopedPointer>

#include <Qt3DRaytrace/qraytraceaspect.h>
#include <Qt3DCore/private/qabstractaspect_p.h>

#include <qt3draytrace_global_p.h>

#include <backend/managers_p.h>

namespace Qt3DRaytrace {

class QRaytraceAspectPrivate : public Qt3DCore::QAbstractAspectPrivate
{
public:
    QRaytraceAspectPrivate();

    void registerBackendTypes();
    void updateServiceProviders();

    QVector<Qt3DCore::QAspectJobPtr> createGeometryRendererJobs() const;

    QScopedPointer<Raytrace::AbstractRenderer> m_renderer;
    QScopedPointer<Raytrace::NodeManagers> m_nodeManagers;

    Q_DECLARE_PUBLIC(QRaytraceAspect)
};

} // Qt3DRaytrace
