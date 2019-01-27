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

#include <jobs/updateworldtransformjob_p.h>
#include <renderers/vulkan/jobs/destroyretiredresourcesjob.h>

#include <QObject>
#include <QSharedPointer>
#include <QVector>
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

    QVector<Geometry> sceneGeometry() const;
    AccelerationStructure sceneTLAS() const;

    void addSceneGeometry(const Geometry &geometry);
    void updateSceneTLAS(const AccelerationStructure &tlas);

    void updateRetiredResources();
    void destroyRetiredResources();

    Raytrace::Entity *sceneRoot() const override;
    void setSceneRoot(Raytrace::Entity *rootEntity) override;
    void setNodeManagers(Raytrace::NodeManagers *nodeManagers) override;

    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const override;
    CommandBufferManager *commandBufferManager() const;

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

    VkQueue m_graphicsQueue = VK_NULL_HANDLE;

    Semaphore m_renderingFinishedSemaphore;
    Semaphore m_presentationFinishedSemaphore;

    RenderPass m_displayRenderPass;
    Pipeline m_displayPipeline;

    CommandPool m_staticCommandPool;
    DescriptorPool m_staticDescriptorPool;
    QueryPool m_queryPool;

    Sampler m_defaultSampler;

    Pipeline m_testPipeline;

    Swapchain m_swapchain;
    DescriptorPool m_swapchainDescriptorPool;
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
        DescriptorSet renderBufferSampleDS;
        DescriptorSet renderBufferStorageDS;
    };
    QVector<FrameResources> m_frameResources;
    int m_frameIndex = 0;

    struct SceneResources {
        AccelerationStructure sceneTLAS;
        QVector<Geometry> geometry;
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
