/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/jobs/uploadtexturejob.h>
#include <renderers/vulkan/renderer.h>

#include <backend/managers_p.h>
#include <backend/textureimage_p.h>

#include <cstring>

namespace Qt3DRaytrace {
namespace Vulkan {

static VkFormat getOptimalTextureFormat(const QImageData &data)
{
    switch(data.type) {
    case QImageDataType::UInt8:
        switch(data.numChannels) {
        case 1: return VK_FORMAT_R8_UNORM;
        case 2: return VK_FORMAT_R8G8_UNORM;
        // Assume sRGB colorspace for RGBA LDR format.
        case 3:
        case 4: return VK_FORMAT_R8G8B8A8_SRGB;
        }
        break;
    case QImageDataType::Float16:
    case QImageDataType::Float32:
        switch(data.numChannels) {
        case 1: return VK_FORMAT_R16_SFLOAT;
        case 2: return VK_FORMAT_R16G16_SFLOAT;
        case 3:
        case 4: return VK_FORMAT_R16G16B16A16_SFLOAT;
        }
        break;
    default:
        break;
    }
    return VK_FORMAT_UNDEFINED;
}

static VkFormat getLinearTextureFormat(const QImageData &data)
{
    switch(data.type) {
    case QImageDataType::UInt8:
        switch(data.numChannels) {
        case 1: return VK_FORMAT_R8_UNORM;
        case 2: return VK_FORMAT_R8G8_UNORM;
        // Assume sRGB colorspace for RGB & RGBA LDR formats.
        case 3: return VK_FORMAT_R8G8B8_SRGB;
        case 4: return VK_FORMAT_R8G8B8A8_SRGB;
        }
        break;
    case QImageDataType::Float16:
        switch(data.numChannels) {
        case 1: return VK_FORMAT_R16_SFLOAT;
        case 2: return VK_FORMAT_R16G16_SFLOAT;
        case 3: return VK_FORMAT_R16G16B16_SFLOAT;
        case 4: return VK_FORMAT_R16G16B16A16_SFLOAT;
        }
        break;
    case QImageDataType::Float32:
        switch(data.numChannels) {
        case 1: return VK_FORMAT_R32_SFLOAT;
        case 2: return VK_FORMAT_R32G32_SFLOAT;
        case 3: return VK_FORMAT_R32G32B32_SFLOAT;
        case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        }
        break;
    default:
        break;
    }
    return VK_FORMAT_UNDEFINED;
}

UploadTextureJob::UploadTextureJob(Renderer *renderer, const Raytrace::HTextureImage &handle)
    : m_renderer(renderer)
    , m_handle(handle)
{
    Q_ASSERT(m_renderer);
}

void UploadTextureJob::run()
{
    Raytrace::TextureImage *textureImageNode = m_handle.data();
    if(!textureImageNode) {
        return;
    }

    const auto &imageData = textureImageNode->data();
    const uint32_t imageWidth = uint32_t(imageData.imageWidth);
    const uint32_t imageHeight = uint32_t(imageData.imageHeight);

    auto *device = m_renderer->device();
    auto *commandBufferManager = m_renderer->commandBufferManager();
    auto *sceneManager = m_renderer->sceneManager();

    VkFormat stagingFormat = getLinearTextureFormat(imageData);
    VkFormat optimalFormat = getOptimalTextureFormat(imageData);
    if(stagingFormat == VK_FORMAT_UNDEFINED || optimalFormat == VK_FORMAT_UNDEFINED) {
        qCCritical(logVulkan) << "UploadTextureJob: unsupported texture image data format";
        return;
    }

    ImageCreateInfo imageCreateInfo;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = optimalFormat;
    imageCreateInfo.extent.width = imageWidth;
    imageCreateInfo.extent.height = imageHeight;
    imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    Image textureImage = device->createImage(imageCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY);
    if(!textureImage) {
        qCCritical(logVulkan) << "Failed to create target image for GPU texture upload";
        return;
    }

    Image stagingImage = device->createStagingImage(imageWidth, imageHeight, stagingFormat);
    if(!stagingImage || !stagingImage.isHostAccessible()) {
        qCCritical(logVulkan) << "Failed to create staging image for GPU texture upload";
        device->destroyImage(textureImage);
        return;
    }

    std::memcpy(stagingImage.hostAddress, imageData.data.data(), size_t(imageData.data.size()));

    TransientCommandBuffer commandBuffer = commandBufferManager->acquireCommandBuffer();
    {
        // TODO: Use plain copy via a staging buffer if format conversion is not necessary.

        VkImageBlit region = {};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.layerCount = 1;
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.layerCount = 1;
        region.srcOffsets[1] = { int32_t(imageWidth), int32_t(imageHeight), 1 };
        region.dstOffsets[1] = { int32_t(imageWidth), int32_t(imageHeight), 1 };

        commandBuffer->resourceBarrier({
            ImageTransition{stagingImage, ImageState::Staging,   ImageState::CopySource},
            ImageTransition{textureImage, ImageState::Undefined, ImageState::CopyDest}
        });
        commandBuffer->blitImage(stagingImage, ImageState::CopySource, textureImage, ImageState::CopyDest, region, VK_FILTER_NEAREST);
        commandBuffer->resourceBarrier(ImageTransition{textureImage, ImageState::CopyDest, ImageState::ShaderRead});
    }
    commandBufferManager->releaseCommandBuffer(commandBuffer, QVector<Image>{stagingImage});

    sceneManager->addOrUpdateTexture(textureImageNode->peerId(), textureImage);
}

} // Vulkan
} // Qt3DRaytrace
