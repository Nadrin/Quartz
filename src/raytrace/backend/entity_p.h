/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <backend/backendnode_p.h>

#include <Qt3DCore/private/qhandle_p.h>
#include <QVector>

namespace Qt3DRaytrace {

class QAbstractRenderer;

namespace Raytrace {

struct NodeManagers;

using HEntity = Qt3DCore::QHandle<class Entity>;

class Entity : public BackendNode
{
    friend class EntityMapper;
    friend class EntityManager;
public:
    ~Entity();

    HEntity handle() const { return m_handle; }

    void setParentHandle(HEntity parentHandle);
    HEntity parentHandle() const { return m_parentHandle; }
    Entity *parent() const;

    void appendChildHandle(HEntity childHandle);
    void removeChildHandle(HEntity childHandle);
    QVector<Entity*> children() const;

    const QVector<HEntity> &childrenHandles() const { return m_childrenHandles; }
    bool hasChildren() const { return !m_childrenHandles.empty(); }

protected:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;

    NodeManagers *m_nodeManagers = nullptr;
    HEntity m_handle;
    HEntity m_parentHandle;
    QVector<HEntity> m_childrenHandles;
};

class EntityMapper : public Qt3DCore::QBackendNodeMapper
{
public:
    EntityMapper(NodeManagers *managers, QAbstractRenderer *renderer);

    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    NodeManagers *m_nodeManagers;
    QAbstractRenderer *m_renderer;
};

} // Raytrace
} // Qt3DRaytrace
