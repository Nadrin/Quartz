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
#include <renderers/vulkan/managers/scenemanager.h>
#include <renderers/vulkan/managers/cameramanager.h>

#include <jobs/updateworldtransformjob_p.h>
#include <renderers/vulkan/jobs/destroyexpiredresourcesjob.h>
#include <renderers/vulkan/jobs/updaterenderparametersjob.h>
#include <renderers/vulkan/jobs/updateinstancebufferjob.h>
#include <renderers/vulkan/jobs/updateemittersjob.h>

#include <utility/movingaverage.h>

#include <Qt3DCore/QNodeId>
#include <QObject>
#include <QSharedPointer>
#include <QReadWriteLock>
#include <QVector>
#include <QHash>
#include <QSize>
#include <QElapsedTimer>

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

    Raytrace::Entity *sceneRoot() const override;
    Raytrace::RenderSettings *settings() const override;

    void setSceneRoot(Raytrace::Entity *rootEntity) override;
    void setSettings(Raytrace::RenderSettings *settings) override;
    void setNodeManagers(Raytrace::NodeManagers *nodeManagers) override;

    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const override;
    CommandBufferManager *commandBufferManager() const;
    DescriptorManager *descriptorManager() const;
    SceneManager *sceneManager() const;
    CameraManager *cameraManager() const;

    QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;

    int currentFrameIndex() const;
    int previousFrameIndex() const;
    uint32_t numConcurrentFrames() const;

private slots:
    void renderFrame();
    void displayStatistics();

private:
    QVector<Qt3DCore::QAspectJobPtr> createGeometryJobs();
    QVector<Qt3DCore::QAspectJobPtr> createTextureJobs();
    QVector<Qt3DCore::QAspectJobPtr> createMaterialJobs(bool forceAllDirty);

    bool createResources();
    void releaseResources();
    void createSwapchainResources();
    void releaseSwapchainResources();
    void releaseWindowSurface();

    void beginRenderIteration();
    void resetRenderProgress();
    void updateActiveCamera();

    bool querySwapchainProperties(VkPhysicalDevice physicalDevice, VkSurfaceFormatKHR &surfaceFormat, int &minImageCount) const;
    bool querySwapchainPresentModes(VkPhysicalDevice physicalDevice, bool vsync, VkPresentModeKHR &presentMode) const;
    void resizeSwapchain();

    bool acquireNextSwapchainImage(uint32_t &imageIndex) const;
    bool submitFrameCommandsAndPresent(uint32_t imageIndex);

    VkPhysicalDevice choosePhysicalDevice(const QByteArrayList &requiredExtensions, uint32_t &queueFamilyIndex) const;
    RenderPass createDisplayRenderPass(VkFormat swapchainFormat) const;

    QVulkanInstance *m_instance = nullptr;
    QWindow *m_window = nullptr;
    QString m_windowTitle;
    mutable QReadWriteLock m_windowSurfaceLock;

    QTimer *m_renderTimer = nullptr;
    QTimer *m_statisticsTimer = nullptr;

    Raytrace::NodeManagers *m_nodeManagers = nullptr;
    Raytrace::RenderSettings *m_settings = nullptr;

    QSharedPointer<Device> m_device;
    QSharedPointer<FrameAdvanceService> m_frameAdvanceService;
    QSharedPointer<CommandBufferManager> m_commandBufferManager;
    QSharedPointer<DescriptorManager> m_descriptorManager;
    QSharedPointer<SceneManager> m_sceneManager;
    QSharedPointer<CameraManager> m_cameraManager;

    VkQueue m_graphicsQueue = VK_NULL_HANDLE;

    Semaphore m_renderingFinishedSemaphore;
    Semaphore m_presentationFinishedSemaphore;

    RenderPass m_displayRenderPass;
    Pipeline m_displayPipeline;
    RayTracingPipeline m_renderPipeline;

    Sampler m_displaySampler;
    Sampler m_textureSampler;
    QueryPool m_defaultQueryPool;

    Swapchain m_swapchain;
    VkSurfaceFormatKHR m_swapchainFormat;
    VkPresentModeKHR m_swapchainPresentMode;
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
    uint32_t m_frameNumber = 0;
    QElapsedTimer m_frameElapsedTimer;

    RenderParameters m_renderParams = {};
    DisplayParameters m_displayParams = {};

    bool m_renderBuffersReady = false;
    bool m_clearPreviousRenderBuffer = false;

    Raytrace::UpdateWorldTransformJobPtr m_updateWorldTransformJob;
    DestroyExpiredResourcesJobPtr m_destroyExpiredResourcesJob;
    UpdateRenderParametersJobPtr m_updateRenderParametersJob;
    UpdateInstanceBufferJobPtr m_updateInstanceBufferJob;
    UpdateEmittersJobPtr m_updateEmittersJob;

    Raytrace::Entity *m_sceneRoot = nullptr;
    DirtySet m_dirtySet = DirtyFlag::AllDirty;

    Utility::MovingAverage<double> m_deviceTimeAverage;
    Utility::MovingAverage<double> m_hostTimeAverage;
};

} // Vulkan
} // Qt3DRaytrace
