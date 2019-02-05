/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/vkresources.h>
#include <renderers/vulkan/geometry.h>
#include <renderers/vulkan/shaders/types.glsl.h>
#include <renderers/vulkan/managers/sceneresourceset.h>

#include <backend/handles_p.h>

#include <QReadWriteLock>

namespace Qt3DRaytrace {

namespace Raytrace {
class EntityManager;
} // Raytrace

namespace Vulkan {

class Renderer;

class SceneManager
{
public:
    explicit SceneManager(Renderer *renderer);
    ~SceneManager();

    void addOrUpdateGeometry(Qt3DCore::QNodeId geometryNodeId, const Geometry &geometry);
    void addOrUpdateMaterial(Qt3DCore::QNodeId materialNodeId, const Material &material);

    void updateSceneTLAS(const AccelerationStructure &tlas);
    void updateMaterialBuffer(const Buffer &buffer);
    void updateInstanceBuffer(const Buffer &buffer);

    uint32_t lookupGeometryIndex(Qt3DCore::QNodeId geometryNodeId) const;
    uint32_t lookupMaterialIndex(Qt3DCore::QNodeId materialNodeId) const;
    uint32_t lookupGeometryBLAS(Qt3DCore::QNodeId geometryNodeId, uint64_t &blasHandle) const;

    void updateRenderables(Raytrace::EntityManager *entityManager);
    void updateRetiredResources();

    void destroyResources();
    void destroyExpiredResources();

    bool isReadyToRender() const;

    AccelerationStructure sceneTLAS() const;
    Buffer instanceBuffer() const;
    Buffer materialBuffer() const;

    const QVector<Raytrace::HEntity> &renderables() const;
    QVector<Material> materials() const;
    QVector<Geometry> geometry() const;

private:
    QVector<Raytrace::HEntity> m_renderables;

    SceneResourceSet<Geometry> m_geometry;
    SceneResourceSet<Material> m_materials;

    ManagedResource<AccelerationStructure> m_tlas;
    ManagedResource<Buffer> m_instanceBuffer;
    ManagedResource<Buffer> m_materialBuffer;

    Renderer *m_renderer;

    mutable QReadWriteLock m_rwlock;
};

} // Vulkan
} // Qt3DRaytrace
