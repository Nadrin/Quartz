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
#include <renderers/vulkan/commandbuffer.h>

#include <jobs/updateworldtransformjob_p.h>

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

    bool initialize() override;
    void shutdown() override;

    void markDirty(DirtySet changes, Raytrace::BackendNode *node) override;

    Raytrace::Entity *sceneRoot() const override;
    void setSceneRoot(Raytrace::Entity *rootEntity) override;

    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const override;
    QVector<Qt3DCore::QAspectJobPtr> renderJobs() override;

private slots:
    void renderFrame();

private:
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

    int currentFrameIndex() const;
    int previousFrameIndex() const;
    uint32_t numConcurrentFrames() const;

    QWindow *m_window = nullptr;
    QVulkanInstance *m_instance = nullptr;
    QTimer *m_renderTimer = nullptr;

    QSharedPointer<Device> m_device;
    QSharedPointer<FrameAdvanceService> m_frameAdvanceService;

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

    bool m_renderBuffersReady = false;
    bool m_clearPreviousRenderBuffer = false;

    Raytrace::UpdateWorldTransformJobPtr m_updateWorldTransformJob;

    Raytrace::Entity *m_sceneRoot = nullptr;
    DirtySet m_dirtySet = DirtyFlag::AllDirty;
};

} // Vulkan
} // Qt3DRaytrace
