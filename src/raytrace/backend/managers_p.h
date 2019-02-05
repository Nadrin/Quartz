/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <private/qresourcemanager_p.h>

#include <backend/entity_p.h>
#include <backend/transform_p.h>
#include <backend/geometry_p.h>
#include <backend/geometryrenderer_p.h>
#include <backend/material_p.h>

#include <QVector>

namespace Qt3DRaytrace {
namespace Raytrace {

class EntityManager : public Qt3DCore::QResourceManager<Entity, Qt3DCore::QNodeId>
{
public:
    ~EntityManager()
    {
        Allocator::for_each([](Entity *e) {
            if(e) {
                e->m_nodeManagers = nullptr;
            }
        });
    }
};

template<typename T, template <class> class LockingPolicy = Qt3DCore::NonLockingPolicy>
class ComponentManager : public Qt3DCore::QResourceManager<T, Qt3DCore::QNodeId, LockingPolicy>
{
public:
    void markComponentDirty(Qt3DCore::QNodeId componentId)
    {
        if(!m_dirtyComponents.contains(componentId)) {
            m_dirtyComponents.append(componentId);
        }
    }
    QVector<Qt3DCore::QNodeId> acquireDirtyComponents()
    {
        QVector<Qt3DCore::QNodeId> result(std::move(m_dirtyComponents));
        return result;
    }

private:
    QVector<Qt3DCore::QNodeId> m_dirtyComponents;
};

class TransformManager : public Qt3DCore::QResourceManager<Transform, Qt3DCore::QNodeId> {};
class GeometryManager : public ComponentManager<Geometry, Qt3DCore::ObjectLevelLockingPolicy> {};
class GeometryRendererManager : public ComponentManager<GeometryRenderer, Qt3DCore::ObjectLevelLockingPolicy> {};
class MaterialManager : public ComponentManager<Material, Qt3DCore::ObjectLevelLockingPolicy> {};

struct NodeManagers
{
    EntityManager entityManager;
    TransformManager transformManager;
    GeometryManager geometryManager;
    GeometryRendererManager geometryRendererManager;
    MaterialManager materialManager;
};

} // Raytrace
} // Qt3DRaytrace
