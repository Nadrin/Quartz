/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/managers/scenemanager.h>
#include <renderers/vulkan/managers/descriptormanager.h>
#include <renderers/vulkan/renderer.h>

#include <backend/managers_p.h>

namespace Qt3DRaytrace {
namespace Vulkan {

SceneManager::SceneManager(Renderer *renderer)
    : m_renderer(renderer)
{
    Q_ASSERT(m_renderer);
}

SceneManager::~SceneManager()
{
    destroyResources();
}

void SceneManager::addOrUpdateGeometry(Qt3DCore::QNodeId geometryNodeId, const Geometry &geometry)
{
    DescriptorManager *descriptorManager = m_renderer->descriptorManager();
    Q_ASSERT(descriptorManager);

    QWriteLocker lock(&m_rwlock);

    DescriptorHandle geometryAttributesDescriptor = descriptorManager->allocateDescriptor(ResourceClass::AttributeBuffer);
    DescriptorHandle geometryIndicesDescriptor = descriptorManager->allocateDescriptor(ResourceClass::IndexBuffer);
    descriptorManager->updateBufferDescriptor(geometryAttributesDescriptor, DescriptorBufferInfo(geometry.attributes));
    descriptorManager->updateBufferDescriptor(geometryIndicesDescriptor, DescriptorBufferInfo(geometry.indices));

    m_geometry.addOrUpdateResource(geometryNodeId, geometry);
}

void SceneManager::addOrUpdateMaterial(Qt3DCore::QNodeId materialNodeId, const Material &material)
{
    QWriteLocker lock(&m_rwlock);
    m_materials.addOrUpdateResource(materialNodeId, material);
}

void SceneManager::updateSceneTLAS(const AccelerationStructure &tlas)
{
    QWriteLocker lock(&m_rwlock);
    m_tlas.update(tlas, m_renderer->numConcurrentFrames());
}

void SceneManager::updateMaterialBuffer(const Buffer &buffer)
{
    QWriteLocker lock(&m_rwlock);
    m_materialBuffer.update(buffer, m_renderer->numConcurrentFrames());
}

void SceneManager::updateInstanceBuffer(const Buffer &buffer)
{
    QWriteLocker lock(&m_rwlock);
    m_instanceBuffer.update(buffer, m_renderer->numConcurrentFrames());
}

uint32_t SceneManager::lookupGeometryIndex(Qt3DCore::QNodeId geometryNodeId) const
{
    QReadLocker lock(&m_rwlock);
    return m_geometry.lookupIndex(geometryNodeId);
}

uint32_t SceneManager::lookupMaterialIndex(Qt3DCore::QNodeId materialNodeId) const
{
    QReadLocker lock(&m_rwlock);
    return m_materials.lookupIndex(materialNodeId);
}

uint32_t SceneManager::lookupGeometryBLAS(Qt3DCore::QNodeId geometryNodeId, uint64_t &blasHandle) const
{
    QReadLocker lock(&m_rwlock);

    Geometry geometry;
    uint32_t geometryIndex = m_geometry.lookupResource(geometryNodeId, geometry);
    blasHandle = geometry.blasHandle;
    return geometryIndex;
}

void SceneManager::updateRenderables(Raytrace::EntityManager *entityManager)
{
    Q_ASSERT(entityManager);

    // NO LOCK: Access from render/aspect thread only.
    m_renderables.clear();
    for(const auto &entity : entityManager->activeHandles()) {
        if(entity->isRenderable()) {
            m_renderables.append(entity->handle());
        }
    }
}

void SceneManager::updateRetiredResources()
{
    // NO LOCK: Access from render/aspect thread only.
    m_tlas.updateRetiredTTL();
    m_instanceBuffer.updateRetiredTTL();
    m_materialBuffer.updateRetiredTTL();
}

void SceneManager::destroyResources()
{
    Device *device = m_renderer->device();
    Q_ASSERT(device);

    // NO LOCK: Access from render/aspect thread only.

    if(m_tlas.resource) {
        device->destroyAccelerationStructure(m_tlas.resource);
        for(auto &retiredTLAS : m_tlas.retired()) {
            device->destroyAccelerationStructure(retiredTLAS);
        }
        m_tlas.reset();
    }
    if(m_instanceBuffer.resource) {
        device->destroyBuffer(m_instanceBuffer.resource);
        for(auto &retiredBuffer : m_instanceBuffer.retired()) {
            device->destroyBuffer(retiredBuffer);
        }
        m_instanceBuffer.reset();
    }
    if(m_materialBuffer.resource) {
        device->destroyBuffer(m_materialBuffer.resource);
        for(auto &retiredBuffer : m_materialBuffer.retired()) {
            device->destroyBuffer(retiredBuffer);
        }
        m_materialBuffer.reset();
    }

    for(auto &geometry : m_geometry.takeResources()) {
        device->destroyGeometry(geometry);
    }
    m_materials.reset();
}

void SceneManager::destroyExpiredResources()
{
    Device *device = m_renderer->device();
    Q_ASSERT(device);

    QWriteLocker lock(&m_rwlock);
    QVarLengthArray<AccelerationStructure> expiredTLAS = m_tlas.takeExpired();
    QVarLengthArray<Buffer> expiredInstanceBuffers = m_instanceBuffer.takeExpired();
    QVarLengthArray<Buffer> expiredMaterialBuffers = m_materialBuffer.takeExpired();
    lock.unlock();

    for(auto &tlas : expiredTLAS) {
        device->destroyAccelerationStructure(tlas);
    }
    for(auto &buffer : expiredInstanceBuffers) {
        device->destroyBuffer(buffer);
    }
    for(auto &buffer : expiredMaterialBuffers) {
        device->destroyBuffer(buffer);
    }
}

bool SceneManager::isReadyToRender() const
{
    // NO LOCK: Access from render/aspect thread only.
    return m_tlas.resource && m_instanceBuffer.resource && m_materialBuffer.resource;
}

const QVector<Raytrace::HEntity> &SceneManager::renderables() const
{
    // NO LOCK: Access from render/aspect thread only.
    return m_renderables;
}

AccelerationStructure SceneManager::sceneTLAS() const
{
    // NO LOCK: Access from render/aspect thread or TLAS build job.
    return m_tlas.resource;
}

Buffer SceneManager::instanceBuffer() const
{
    // NO LOCK: Access from render/aspect thread only.
    return m_instanceBuffer.resource;
}

Buffer SceneManager::materialBuffer() const
{
    // NO LOCK: Access from render/aspect thread only.
    return m_materialBuffer.resource;
}

QVector<Material> SceneManager::materials() const
{
    QReadLocker lock(&m_rwlock);
    return m_materials.resources();
}

QVector<Geometry> SceneManager::geometry() const
{
    QReadLocker lock(&m_rwlock);
    return m_geometry.resources();
}

} // Vulkan
} // Qt3DRaytrace
