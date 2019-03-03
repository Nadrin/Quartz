/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <backend/rendersettings_p.h>
#include <frontend/qrendersettings_p.h>

#include <Qt3DCore/QPropertyUpdatedChange>

#include <limits>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

RenderSettings::RenderSettings()
    : BackendNode(BackendNode::ReadOnly)
{}

float RenderSettings::directRadianceClamp() const
{
    return (m_directRadianceClamp > 0.0f)
            ? m_directRadianceClamp
            : std::numeric_limits<float>::infinity();
}

float RenderSettings::indirectRadianceClamp() const
{
    return (m_indirectRadianceClamp > 0.0f)
            ? m_indirectRadianceClamp
            : std::numeric_limits<float>::infinity();
}

void RenderSettings::sceneChangeEvent(const QSceneChangePtr &change)
{
    if(change->type() == PropertyUpdated) {
        QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(change);
        if(propertyChange->propertyName() == QByteArrayLiteral("camera")) {
            m_cameraId = propertyChange->value().value<QNodeId>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("primarySamples")) {
            m_primarySamples = propertyChange->value().value<unsigned int>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("secondarySamples")) {
            m_secondarySamples = propertyChange->value().value<unsigned int>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("minDepth")) {
            m_minDepth = propertyChange->value().value<unsigned int>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("maxDepth")) {
            m_maxDepth = propertyChange->value().value<unsigned int>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("directRadianceClamp")) {
            m_directRadianceClamp = propertyChange->value().value<float>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("indirectRadianceClamp")) {
            m_indirectRadianceClamp = propertyChange->value().value<float>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("skyColor")) {
            m_skyColor = propertyChange->value().value<QColor>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("skyIntensity")) {
            m_skyIntensity = propertyChange->value().value<float>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("skyTexture")) {
            m_skyTextureId = propertyChange->value().value<QNodeId>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("skyTextureOffset")) {
            m_skyTextureOffset = propertyChange->value().value<QVector2D>();
        }

        markDirty(AbstractRenderer::AllDirty);
    }
    BackendNode::sceneChangeEvent(change);
}

void RenderSettings::initializeFromPeer(const QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QRenderSettingsData>>(change);
    const auto &data = typedChange->data;

    m_cameraId = data.cameraId;
    m_primarySamples = static_cast<unsigned int>(data.primarySamples);
    m_secondarySamples = static_cast<unsigned int>(data.secondarySamples);
    m_minDepth = static_cast<unsigned int>(data.minDepth);
    m_maxDepth = static_cast<unsigned int>(data.maxDepth);
    m_directRadianceClamp = data.directRadianceClamp;
    m_indirectRadianceClamp = data.indirectRadianceClamp;

    m_skyColor = data.skyColor;
    m_skyIntensity = data.skyIntensity;
    m_skyTextureId = data.skyTextureId;
    m_skyTextureOffset = data.skyTextureOffset;

    markDirty(AbstractRenderer::AllDirty);
}

QBackendNode *RenderSettingsMapper::create(const QNodeCreatedChangeBasePtr &change) const
{
    Q_UNUSED(change);

    if(m_renderer->settings() != nullptr) {
        qCWarning(logAspect) << "RenderSettings component already exists";
        return nullptr;
    }

    RenderSettings *settings = new RenderSettings;
    settings->setRenderer(m_renderer);
    m_renderer->setSettings(settings);
    return settings;
}

QBackendNode *RenderSettingsMapper::get(QNodeId id) const
{
    Q_UNUSED(id);
    return m_renderer->settings();
}

void RenderSettingsMapper::destroy(QNodeId id) const
{
    Q_UNUSED(id);
    RenderSettings *settings = m_renderer->settings();
    m_renderer->setSettings(nullptr);
    delete settings;
}

} // Raytrace
} // Qt3DRaytrace
