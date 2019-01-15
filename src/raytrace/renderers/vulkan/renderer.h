/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <backend/abstractrenderer_p.h>

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/initializers.h>
#include <renderers/vulkan/device.h>
#include <renderers/vulkan/frameadvanceservice.h>

#include <jobs/updateworldtransformjob_p.h>

#include <QScopedPointer>
#include <QVector>

class QVulkanWindow;

namespace Qt3DRaytrace {
namespace Vulkan {

class Renderer final : public Raytrace::AbstractRenderer
{
public:
    Renderer();

    void preInitResources() override;
    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;
    void startNextFrame() override;

    API api() const override { return QRendererInterface::API::Vulkan; }

    QSurface *surface() const override;
    void setSurface(QObject *surfaceObject) override;

    void markDirty(DirtySet changes, Raytrace::BackendNode *node) override;

    Raytrace::Entity *sceneRoot() const override
    {
        return m_sceneRoot;
    }
    void setSceneRoot(Raytrace::Entity *rootEntity) override;

    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const override;
    QVector<Qt3DCore::QAspectJobPtr> renderJobs() override;

private:
    QVulkanWindow *m_window = nullptr;
    QScopedPointer<FrameAdvanceService> m_frameAdvanceService;
    QScopedPointer<Device> m_device;

    Raytrace::UpdateWorldTransformJobPtr m_updateWorldTransformJob;

    Raytrace::Entity *m_sceneRoot = nullptr;
    DirtySet m_dirtySet = DirtyFlag::AllDirty;

    struct FrameResources {
        Image renderBuffer;
        DescriptorSet renderBufferSampleDS;
        DescriptorSet renderBufferStorageDS;
    };
    QVector<FrameResources> m_frameResources;
    bool m_renderBuffersReady = false;
    bool m_clearPreviousRenderBuffer = false;

    DescriptorPool m_defaultDescriptorPool;
    DescriptorPool m_swapChainDescriptorPool;
    QueryPool m_queryPool;
    Sampler m_defaultSampler;

    Pipeline m_displayPipeline;
    Pipeline m_testPipeline;
};

} // Vulkan
} // Qt3DRaytrace
