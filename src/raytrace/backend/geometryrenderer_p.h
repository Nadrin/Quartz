/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <backend/backendnode_p.h>
#include <Qt3DRaytrace/qgeometryfactory.h>

namespace Qt3DRaytrace {
namespace Raytrace {

class GeometryRendererManager;

class GeometryRenderer : public BackendNode
{
public:
    GeometryRenderer();

    void setManager(GeometryRendererManager *manager);
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;
    void loadGeometry();

    Qt3DCore::QNodeId geometryId() const { return m_geometryId; }
    QGeometryFactoryPtr geometryFactory() const { return m_geometryFactory; }

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;

    GeometryRendererManager *m_manager;
    Qt3DCore::QNodeId m_geometryId;
    QGeometryFactoryPtr m_geometryFactory;
};

class GeometryRendererNodeMapper final : public BackendNodeMapper<GeometryRenderer, GeometryRendererManager>
{
public:
    GeometryRendererNodeMapper(GeometryRendererManager *manager, AbstractRenderer *renderer)
        : BackendNodeMapper(manager, renderer)
    {}

    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const override
    {
        auto geometryRenderer = static_cast<GeometryRenderer*>(BackendNodeMapper::create(change));
        geometryRenderer->setManager(m_manager);
        return geometryRenderer;
    }
};

} // Raytrace
} // Qt3DRaytrace
