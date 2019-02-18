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

void SceneManager::updateEmitters(QVector<Emitter> &emitters)
{
    QWriteLocker lock(&m_rwlock);
    m_emitters = std::move(emitters);
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

void SceneManager::updateEmitterBuffer(const Buffer &buffer)
{
    QWriteLocker lock(&m_rwlock);
    m_emitterBuffer.update(buffer, m_renderer->numConcurrentFrames());
}

void SceneManager::updateInstanceBuffer(const Buffer &buffer)
{
    QWriteLocker lock(&m_rwlock);
    m_instanceBuffer.update(buffer, m_renderer->numConcurrentFrames());
}

uint32_t SceneManager::lookupGeometry(Qt3DCore::QNodeId geometryNodeId, Geometry &geometry) const
{
    QReadLocker lock(&m_rwlock);
    return m_geometry.lookupResource(geometryNodeId, geometry);
}

uint32_t SceneManager::lookupGeometryIndex(Qt3DCore::QNodeId geometryNodeId) const
{
    QReadLocker lock(&m_rwlock);
    return m_geometry.lookupIndex(geometryNodeId);
}

uint32_t SceneManager::lookupMaterial(Qt3DCore::QNodeId materialNodeId, Material &material) const
{
    QReadLocker lock(&m_rwlock);
    return m_materials.lookupResource(materialNodeId, material);
}

uint32_t SceneManager::lookupMaterialIndex(Qt3DCore::QNodeId materialNodeId) const
{
    QReadLocker lock(&m_rwlock);
    return m_materials.lookupIndex(materialNodeId);
}

void SceneManager::gatherEntities(Raytrace::EntityManager *entityManager)
{
    Q_ASSERT(entityManager);

    // NO LOCK: Access from render/aspect thread only.
    m_renderables.clear();
    m_emissives.clear();
    for(const auto &entity : entityManager->activeHandles()) {
        if(entity->isRenderable()) {
            m_renderables.addResource(entity->peerId(), entity->handle());
        }
        if(entity->isEmissive()) {
            m_emissives.addResource(entity->peerId(), entity->handle());
        }
    }
}

void SceneManager::updateRetiredResources()
{
    // NO LOCK: Access from render/aspect thread only.
    m_tlas.updateRetiredTTL();
    m_instanceBuffer.updateRetiredTTL();
    m_materialBuffer.updateRetiredTTL();
    m_emitterBuffer.updateRetiredTTL();
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
    if(m_emitterBuffer.resource) {
        device->destroyBuffer(m_emitterBuffer.resource);
        for(auto &retiredBuffer : m_emitterBuffer.retired()) {
            device->destroyBuffer(retiredBuffer);
        }
        m_emitterBuffer.reset();
    }

    for(auto &geometry : m_geometry.takeResources()) {
        device->destroyGeometry(geometry);
    }
    m_materials.clear();
}

void SceneManager::destroyExpiredResources()
{
    Device *device = m_renderer->device();
    Q_ASSERT(device);

    QWriteLocker lock(&m_rwlock);
    QVarLengthArray<AccelerationStructure> expiredTLAS = m_tlas.takeExpired();
    QVarLengthArray<Buffer> expiredInstanceBuffers = m_instanceBuffer.takeExpired();
    QVarLengthArray<Buffer> expiredMaterialBuffers = m_materialBuffer.takeExpired();
    QVarLengthArray<Buffer> expiredEmitterBuffers = m_emitterBuffer.takeExpired();
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
    for(auto &buffer : expiredEmitterBuffers) {
        device->destroyBuffer(buffer);
    }
}

bool SceneManager::isReadyToRender() const
{
    // NO LOCK: Access from render/aspect thread only.
    return m_tlas.resource &&
           m_instanceBuffer.resource &&
           m_materialBuffer.resource &&
           m_emitterBuffer.resource;
}

const QVector<Raytrace::HEntity> &SceneManager::renderables() const
{
    // NO LOCK: Access from render/aspect thread only.
    return m_renderables.resources();
}

const QVector<Raytrace::HEntity> &SceneManager::emissives() const
{
    // NO LOCK: Access from render/aspect thread only.
    return m_emissives.resources();
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

Buffer SceneManager::emitterBuffer() const
{
    // NO LOCK: Access from render/aspect thread only.
    return m_emitterBuffer.resource;
}

uint32_t SceneManager::lookupRenderableIndex(Qt3DCore::QNodeId entityNodeId) const
{
    QReadLocker lock(&m_rwlock);
    return m_renderables.lookupIndex(entityNodeId);
}

uint32_t SceneManager::lookupEmissiveIndex(Qt3DCore::QNodeId entityNodeId) const
{
    QReadLocker lock(&m_rwlock);
    return m_emissives.lookupIndex(entityNodeId);
}

QVector<Material> SceneManager::materials() const
{
    QReadLocker lock(&m_rwlock);
    auto result = m_materials.resources();
    result.detach();
    return result;
}

QVector<Geometry> SceneManager::geometry() const
{
    QReadLocker lock(&m_rwlock);
    auto result = m_geometry.resources();
    result.detach();
    return result;
}

QVector<Emitter> SceneManager::emitters() const
{
    QReadLocker lock(&m_rwlock);
    auto result = m_emitters;
    result.detach();
    return result;
}

uint32_t SceneManager::numMaterials() const
{
    QReadLocker lock(&m_rwlock);
    return uint32_t(m_materials.resources().size());
}

uint32_t SceneManager::numGeometry() const
{
    QReadLocker lock(&m_rwlock);
    return uint32_t(m_geometry.resources().size());
}

uint32_t SceneManager::numEmitters() const
{
    QReadLocker lock(&m_rwlock);
    return uint32_t(m_emitters.size());
}

} // Vulkan
} // Qt3DRaytrace
