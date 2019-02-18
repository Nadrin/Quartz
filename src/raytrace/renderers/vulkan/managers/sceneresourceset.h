/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <QVector>
#include <QHash>
#include <Qt3DCore/QNodeId>

namespace Qt3DRaytrace {
namespace Vulkan {

template<typename T>
class SceneResourceSet
{
public:
    uint32_t addResource(Qt3DCore::QNodeId nodeId, const T &resource)
    {
        Q_ASSERT(!m_nodeToIndexMap.contains(nodeId));
        uint32_t index = uint32_t(m_resources.size());
        m_resources.append(resource);
        m_nodeToIndexMap.insert(nodeId, index);
        return index;
    }

    uint32_t addOrUpdateResource(Qt3DCore::QNodeId nodeId, const T &resource)
    {
        uint32_t index;
        auto it = m_nodeToIndexMap.find(nodeId);
        if(it == m_nodeToIndexMap.end()) {
            index = uint32_t(m_resources.size());
            m_resources.append(resource);
            m_nodeToIndexMap.insert(nodeId, index);
        }
        else {
            index = *it;
            m_resources[int(index)] = resource;
        }
        return index;
    }

    uint32_t lookupIndex(Qt3DCore::QNodeId nodeId) const
    {
        return m_nodeToIndexMap.value(nodeId, ~0u);
    }

    uint32_t lookupResource(Qt3DCore::QNodeId nodeId, T &resource) const
    {
        uint32_t index = lookupIndex(nodeId);
        if(index != ~0u) {
            resource = m_resources[int(index)];
        }
        return index;
    }

    const QVector<T> &resources() const
    {
        return m_resources;
    }

    QVector<T> takeResources()
    {
        QVector<T> result(std::move(m_resources));
        m_nodeToIndexMap.clear();
        return result;
    }

    void clear()
    {
        m_resources.clear();
        m_nodeToIndexMap.clear();
    }

private:
    QVector<T> m_resources;
    QHash<Qt3DCore::QNodeId, uint32_t> m_nodeToIndexMap;
};

} // Vulkan
} // Qt3DRaytrace
