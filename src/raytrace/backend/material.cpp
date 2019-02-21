/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <backend/material_p.h>
#include <backend/managers_p.h>
#include <frontend/qmaterial_p.h>

#include <Qt3DCore/QPropertyUpdatedChange>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

Material::Material()
    : BackendNode(ReadOnly)
{}

void Material::setManager(MaterialManager *manager)
{
    Q_ASSERT(manager);
    m_manager = manager;
}

void Material::sceneChangeEvent(const QSceneChangePtr &change)
{
    if(change->type() == PropertyUpdated) {
        QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(change);
        const auto propertyName = propertyChange->propertyName();
        if(propertyName == QByteArrayLiteral("albedo")) {
            m_albedo = propertyChange->value().value<QColor>();
        }
        else if(propertyName == QByteArrayLiteral("roughness")) {
            m_roughness = qBound(Material::MinRoughness, propertyChange->value().value<float>(), 1.0f);
        }
        else if(propertyName == QByteArrayLiteral("metalness")) {
            m_metalness = qBound(0.0f, propertyChange->value().value<float>(), 1.0f);
        }
        else if(propertyName == QByteArrayLiteral("emission")) {
            m_emission = propertyChange->value().value<QColor>();
        }
        else if(propertyName == QByteArrayLiteral("emissionIntensity")) {
            m_emissionIntensity = qMax(propertyChange->value().value<float>(), 0.0f);
        }

        if(m_manager) {
            m_manager->markComponentDirty(peerId());
        }
    }

    markDirty(AbstractRenderer::MaterialDirty);
    BackendNode::sceneChangeEvent(change);
}

void Material::initializeFromPeer(const QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<QNodeCreatedChange<QMaterialData>>(change);
    const auto &data = typedChange->data;

    m_albedo = data.albedo;
    m_roughness = qBound(Material::MinRoughness, data.roughness, 1.0f);
    m_metalness = qBound(0.0f, data.metalness, 1.0f);
    m_emission = data.emission;
    m_emissionIntensity = qMax(data.emissionIntensity, 0.0f);

    if(m_manager) {
        m_manager->markComponentDirty(peerId());
    }

    markDirty(AbstractRenderer::MaterialDirty);
}

} // Raytrace
} // Qt3DRaytrace
