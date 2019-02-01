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
#include <renderers/vulkan/commandbuffer.h>
#include <renderers/vulkan/frameadvanceservice.h>
#include <renderers/vulkan/managers/commandbuffermanager.h>
#include <renderers/vulkan/managers/descriptormanager.h>

#include <jobs/updateworldtransformjob_p.h>
#include <renderers/vulkan/jobs/destroyretiredresourcesjob.h>

#include <Qt3DCore/QNodeId>
#include <QObject>
#include <QSharedPointer>
#include <QVector>
#include <QHash>
#include <QSize>

class QWindow;
class QTimer;

namespace Qt3DRaytrace {
namespace Vulkan {

class Renderer final : public QObject
                     , public Raytrace::AbstractRenderer
{
    Q_OBJECT
public:
    explicit Renderer(QObject *parent = nullptr);

    API api() const override { return API::Vulkan; }

    QSurface *surface() const override;
    void setSurface(QObject *surfaceObject) override;

    Device *device() const;

    bool initialize() override;
    void shutdown() override;

    void markDirty(DirtySet changes, Raytrace::BackendNode *node) override;

    QVector<Geometry> geometry() const;
    AccelerationStructure sceneTLAS() const;

    void addGeometry(Qt3DCore::QNodeId geometryNodeId, const Geometry &geometry);
    uint32_t lookupGeometryBLAS(Qt3DCore::QNodeId geometryNodeId, uint64_t &blasHandle) const;
    void updateSceneTLAS(const AccelerationStructure &tlas);

    void updateRetiredResources();
    void destroyRetiredResources();

    Raytrace::Entity *sceneRoot() const override;
    void setSceneRoot(Raytrace::Entity *rootEntity) override;
    void setNodeManagers(Raytrace::NodeManagers *nodeManagers) override;

    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const override;
    CommandBufferManager *commandBufferManager() const;
    DescriptorManager *descriptorManager() const;

    QVector<Qt3DCore::QAspectJobPtr> renderJobs() override;

    int currentFrameIndex() const;
    int previousFrameIndex() const;
    uint32_t numConcurrentFrames() const;

private slots:
    void renderFrame();

private:
    QVector<Qt3DCore::QAspectJobPtr> createGeometryJobs();

    bool createResources();
    void releaseResources();
    void createSwapchainResources();
    void releaseSwapchainResources();

    bool querySwapchainProperties(VkPhysicalDevice physicalDevice, VkSurfaceFormatKHR &surfaceFormat, int &minImageCount) const;
    void resizeSwapchain();

    bool acquireNextSwapchainImage(uint32_t &imageIndex) const;
    bool submitFrameCommandsAndPresent(uint32_t imageIndex);

    VkPhysicalDevice choosePhysicalDevice(const QByteArrayList &requiredExtensions, uint32_t &queueFamilyIndex) const;
    RenderPass createDisplayRenderPass(VkFormat swapchainFormat) const;

    QWindow *m_window = nullptr;
    QVulkanInstance *m_instance = nullptr;
    QTimer *m_renderTimer = nullptr;

    Raytrace::NodeManagers *m_nodeManagers = nullptr;

    QSharedPointer<Device> m_device;
    QSharedPointer<FrameAdvanceService> m_frameAdvanceService;
    QSharedPointer<CommandBufferManager> m_commandBufferManager;
    QSharedPointer<DescriptorManager> m_descriptorManager;

    VkQueue m_graphicsQueue = VK_NULL_HANDLE;

    Semaphore m_renderingFinishedSemaphore;
    Semaphore m_presentationFinishedSemaphore;

    RenderPass m_displayRenderPass;
    Pipeline m_displayPipeline;
    RayTracingPipeline m_rayTracingPipeline;

    Sampler m_defaultSampler;

    Swapchain m_swapchain;
    VkSurfaceFormatKHR m_swapchainFormat;
    QSize m_swapchainSize;

    struct SwapchainAttachment {
        Image image;
        Framebuffer framebuffer;
    };
    QVector<SwapchainAttachment> m_swapchainAttachments;

    struct FrameResources {
        CommandBuffer commandBuffer;
        Fence commandBuffersExecutedFence;
        Image renderBuffer;
        DescriptorSet renderDescriptorSet;
        DescriptorSet displayDescriptorSet;
    };
    QVector<FrameResources> m_frameResources;
    CommandPool m_frameCommandPool;
    DescriptorPool m_frameDescriptorPool;
    int m_frameIndex = 0;

    struct SceneResources {
        AccelerationStructure sceneTLAS;
        QVector<Geometry> geometry;
        QHash<Qt3DCore::QNodeId, uint32_t> geometryIndexLookup;
        QVector<RetiredResource<AccelerationStructure>> retiredTLAS;
    };
    SceneResources m_sceneResources;
    mutable QMutex m_sceneMutex;

    bool m_renderBuffersReady = false;
    bool m_clearPreviousRenderBuffer = false;

    Raytrace::UpdateWorldTransformJobPtr m_updateWorldTransformJob;
    DestroyRetiredResourcesJobPtr m_destroyRetiredResourcesJob;

    Raytrace::Entity *m_sceneRoot = nullptr;
    DirtySet m_dirtySet = DirtyFlag::AllDirty;
};

} // Vulkan
} // Qt3DRaytrace
