/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <Qt3DCore/QBackendNode>

namespace Qt3DRaytrace {

class QAbstractRenderer;

namespace Raytrace {

class BackendNode : public Qt3DCore::QBackendNode
{
public:
    explicit BackendNode(Qt3DCore::QBackendNode::Mode mode = ReadOnly);

    void setRenderer(QAbstractRenderer *renderer)
    {
        m_renderer = renderer;
    }

protected:
    QAbstractRenderer *m_renderer = nullptr;
};

template<typename BackendNodeType, typename ManagerType>
class BackendNodeMapper : public Qt3DCore::QBackendNodeMapper
{
public:
    BackendNodeMapper(ManagerType *manager, QAbstractRenderer *renderer)
        : m_manager(manager)
        , m_renderer(renderer)
    {}

    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const override
    {
        BackendNodeType *backendNode = m_manager->getOrCreateResource(change->subjectId());
        backendNode->setRenderer(m_renderer);
        return backendNode;
    }

    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override
    {
        return m_manager->lookupResource(id);
    }

    void destroy(Qt3DCore::QNodeId id) const override
    {
        m_manager->releaseResource(id);
    }

private:
    ManagerType *m_manager;
    QAbstractRenderer *m_renderer;
};

} // Raytrace
} // Qt3DRaytrace
