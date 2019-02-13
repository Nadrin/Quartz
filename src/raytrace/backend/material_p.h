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
        return LinearColor(m_albedo);
    }
    LinearColor emission() const
    {
        return LinearColor(m_emission, m_emissionPower);
    }

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;

    MaterialManager *m_manager;
    QColor m_albedo;
    QColor m_emission;
    float m_emissionPower;
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
