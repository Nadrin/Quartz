/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <backend/backendnode_p.h>
#include <backend/types_p.h>

namespace Qt3DRaytrace {
namespace Raytrace {

class MaterialManager;

class Material : public BackendNode
{
public:
    Material();

    void setManager(MaterialManager *manager);
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

    LinearColor albedo() const
    {
        return LinearColor::from_sRGB(m_albedo);
    }
    float roughness() const
    {
        return m_roughness;
    }
    float metalness() const
    {
        return m_metalness;
    }
    LinearColor emission() const
    {
        return LinearColor(m_emission, m_emissionIntensity);
    }

    Qt3DCore::QNodeId albedoTextureId() const
    {
        return m_albedoTextureId;
    }
    Qt3DCore::QNodeId roughnessTextureId() const
    {
        return m_roughnessTextureId;
    }
    Qt3DCore::QNodeId metalnessTextureId() const
    {
        return m_metalnessTextureId;
    }

    static constexpr float MinRoughness = 0.02f;

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;

    MaterialManager *m_manager;

    QColor m_albedo;
    float m_roughness;
    float m_metalness;
    QColor m_emission;
    float m_emissionIntensity;

    Qt3DCore::QNodeId m_albedoTextureId;
    Qt3DCore::QNodeId m_roughnessTextureId;
    Qt3DCore::QNodeId m_metalnessTextureId;
};

class MaterialNodeMapper final : public BackendNodeMapper<Material, MaterialManager>
{
public:
    MaterialNodeMapper(MaterialManager *manager, AbstractRenderer *renderer)
        : BackendNodeMapper(manager, renderer)
    {}

    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const override
    {
        auto material = static_cast<Material*>(BackendNodeMapper::create(change));
        material->setManager(m_manager);
        return material;
    }
};

} // Raytrace
} // Qt3DRaytrace
