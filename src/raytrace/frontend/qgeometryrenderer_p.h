/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qgeometryrenderer.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/private/qtypedpropertyupdatechange_p.h>

#include <memory>

namespace Qt3DRaytrace {

class QGeometryRendererPrivate : public Qt3DCore::QComponentPrivate
{
public:
    Q_DECLARE_PUBLIC(QGeometryRenderer)
    QGeometry *m_geometry = nullptr;
    QGeometryFactoryPtr m_geometryFactory;
};

struct QGeometryRendererData
{
    Qt3DCore::QNodeId geometryId;
    QGeometryFactoryPtr geometryFactory;
};

class QGeometry;

// TODO: Is std::unique_ptr really needed here?
using QGeometryChange = Qt3DCore::QTypedPropertyUpdatedChange<std::unique_ptr<QGeometry>>;
using QGeometryChangePtr = Qt3DCore::QTypedPropertyUpdatedChangePtr<std::unique_ptr<QGeometry>>;

} // Qt3DRaytrace
