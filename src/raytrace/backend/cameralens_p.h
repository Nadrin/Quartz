/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <backend/backendnode_p.h>

namespace Qt3DRaytrace {
namespace Raytrace {

class CameraLens : public BackendNode
{
public:
    CameraLens();

    float fieldOfView() const { return m_fieldOfView; }
    float aspectRatio() const { return m_aspectRatio; }
    float gamma() const { return m_gamma; }
    float exposure() const { return m_exposure; }

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;

    float m_fieldOfView;
    float m_aspectRatio;
    float m_gamma;
    float m_exposure;
};

} // Raytrace
} // Qt3DRaytrace
