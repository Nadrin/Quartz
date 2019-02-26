/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/vkresources.h>
#include <renderers/vulkan/geometry.h>
#include <renderers/vulkan/glsl.h>
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
    void addOrUpdateTexture(Qt3DCore::QNodeId textureImageNodeId, const Image &textureImage);
    void updateEmitters(QVector<Emitter> &emitters);

    void updateSceneTLAS(const AccelerationStructure &tlas, uint32_t instanceCount);
    void updateMaterialBuffer(const Buffer &buffer);
    void updateEmitterBuffer(const Buffer &buffer);
    void updateInstanceBuffer(const Buffer &buffer);

    uint32_t lookupGeometry(Qt3DCore::QNodeId geometryNodeId, Geometry &geometry) const;
    uint32_t lookupGeometryIndex(Qt3DCore::QNodeId geometryNodeId) const;
    uint32_t lookupMaterial(Qt3DCore::QNodeId materialNodeId, Material &material) const;
    uint32_t lookupMaterialIndex(Qt3DCore::QNodeId materialNodeId) const;
    uint32_t lookupTextureIndex(Qt3DCore::QNodeId textureImageNodeId) const;

    void gatherEntities(Raytrace::EntityManager *entityManager);
    void updateRetiredResources();

    void destroyResources();
    void destroyExpiredResources();

    bool isReadyToRender() const;

    AccelerationStructure sceneTLAS(uint32_t *instanceCount=nullptr) const;
    Buffer instanceBuffer() const;
    Buffer materialBuffer() const;
    Buffer emitterBuffer() const;

    uint32_t lookupRenderableIndex(Qt3DCore::QNodeId entityNodeId) const;
    uint32_t lookupEmissiveIndex(Qt3DCore::QNodeId entityNodeId) const;

    const QVector<Raytrace::HEntity> &renderables() const;
    const QVector<Raytrace::HEntity> &emissives() const;

    QVector<Material> materials() const;
    QVector<Geometry> geometry() const;
    QVector<Emitter> emitters() const;

    uint32_t numMaterials() const;
    uint32_t numGeometry() const;
    uint32_t numTextures() const;
    uint32_t numEmitters() const;

private:
    SceneResourceSet<Raytrace::HEntity> m_renderables;
    SceneResourceSet<Raytrace::HEntity> m_emissives;

    SceneResourceSet<Geometry> m_geometry;
    SceneResourceSet<Material> m_materials;
    SceneResourceSet<Image> m_textures;
    QVector<Emitter> m_emitters;

    ManagedResource<AccelerationStructure> m_tlas;
    uint32_t m_tlasInstanceCount;

    ManagedResource<Buffer> m_instanceBuffer;
    ManagedResource<Buffer> m_materialBuffer;
    ManagedResource<Buffer> m_emitterBuffer;

    Renderer *m_renderer;

    mutable QReadWriteLock m_rwlock;
};

} // Vulkan
} // Qt3DRaytrace
