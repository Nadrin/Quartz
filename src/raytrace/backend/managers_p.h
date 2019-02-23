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
#include <backend/abstracttexture_p.h>
#include <backend/textureimage_p.h>
#include <backend/material_p.h>
#include <backend/distantlight_p.h>
#include <backend/cameralens_p.h>

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
    void clearDirtyComponents()
    {
        m_dirtyComponents.clear();
    }

private:
    QVector<Qt3DCore::QNodeId> m_dirtyComponents;
};

class TransformManager : public Qt3DCore::QResourceManager<Transform, Qt3DCore::QNodeId> {};
class GeometryManager : public ComponentManager<Geometry> {};
class GeometryRendererManager : public ComponentManager<GeometryRenderer> {};
class TextureManager : public ComponentManager<AbstractTexture> {};
class TextureImageManager : public ComponentManager<TextureImage> {};
class MaterialManager : public ComponentManager<Material> {};
class DistantLightManager : public ComponentManager<DistantLight> {};
class CameraManager : public ComponentManager<CameraLens> {};

struct NodeManagers
{
    EntityManager entityManager;
    TransformManager transformManager;
    GeometryManager geometryManager;
    GeometryRendererManager geometryRendererManager;
    TextureManager textureManager;
    TextureImageManager textureImageManager;
    MaterialManager materialManager;
    DistantLightManager distantLightManager;
    CameraManager cameraManager;
};

} // Raytrace
} // Qt3DRaytrace
