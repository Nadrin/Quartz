/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <backend/rendersettings_p.h>
#include <frontend/qrendersettings_p.h>

#include <Qt3DCore/QPropertyUpdatedChange>

using namespace Qt3DCore;

namespace Qt3DRaytrace {
namespace Raytrace {

RenderSettings::RenderSettings()
    : BackendNode(BackendNode::ReadOnly)
{}

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
        else if(propertyChange->propertyName() == QByteArrayLiteral("skyColor")) {
            m_skyColor = propertyChange->value().value<QColor>();
        }
        else if(propertyChange->propertyName() == QByteArrayLiteral("skyIntensity")) {
            m_skyIntensity = propertyChange->value().value<float>();
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
    m_skyColor = data.skyColor;
    m_skyIntensity = data.skyIntensity;

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
