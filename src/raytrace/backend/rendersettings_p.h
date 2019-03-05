/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <backend/backendnode_p.h>
#include <backend/types_p.h>

#include <QColor>
#include <QVector2D>

namespace Qt3DRaytrace {
namespace Raytrace {

class RenderSettings : public BackendNode
{
public:
    RenderSettings();

    Qt3DCore::QNodeId cameraId() const { return m_cameraId; }
    unsigned int primarySamples() const { return m_primarySamples; }
    unsigned int secondarySamples() const { return m_secondarySamples; }
    unsigned int minDepth() const { return m_minDepth; }
    unsigned int maxDepth() const { return m_maxDepth; }

    float directRadianceClamp() const;
    float indirectRadianceClamp() const;

    LinearColor skyRadiance() const
    {
        return LinearColor::from_sRgb(m_skyColor, m_skyIntensity);
    }

    float skyIntensity() const { return m_skyIntensity; }
    Qt3DCore::QNodeId skyTextureId() const { return m_skyTextureId; }
    QVector2D skyTextureOffset() const { return m_skyTextureOffset; }

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;

    Qt3DCore::QNodeId m_cameraId;
    unsigned int m_primarySamples;
    unsigned int m_secondarySamples;
    unsigned int m_minDepth;
    unsigned int m_maxDepth;
    float m_directRadianceClamp;
    float m_indirectRadianceClamp;
    QColor m_skyColor;
    float m_skyIntensity;
    Qt3DCore::QNodeId m_skyTextureId;
    QVector2D m_skyTextureOffset;
};

class RenderSettingsMapper final : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit RenderSettingsMapper(AbstractRenderer *renderer)
        : m_renderer(renderer)
    {
        Q_ASSERT(m_renderer);
    }

    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    AbstractRenderer *m_renderer;
};

} // Raytrace
} // Qt3DRaytrace
