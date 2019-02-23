/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DRaytrace/qmaterial.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/private/qtypedpropertyupdatechange_p.h>

namespace Qt3DRaytrace {

struct QMaterialData
{
    QColor albedo;
    float roughness;
    float metalness;
    QColor emission;
    float emissionIntensity;
    Qt3DCore::QNodeId albedoTextureId;
    Qt3DCore::QNodeId roughnessTextureId;
    Qt3DCore::QNodeId metalnessTextureId;
};

class QMaterialPrivate : public Qt3DCore::QComponentPrivate
{
public:
    Q_DECLARE_PUBLIC(QMaterial)

    QColor m_albedo = Qt::GlobalColor::gray;
    float m_roughness = 1.0f;
    float m_metalness = 0.0f;
    QColor m_emission = Qt::GlobalColor::white;
    float m_emissionIntensity = 0.0f;
    QAbstractTexture *m_albedoTexture = nullptr;
    QAbstractTexture *m_roughnessTexture = nullptr;
    QAbstractTexture *m_metalnessTexture = nullptr;
};

} // Qt3DRaytrace
