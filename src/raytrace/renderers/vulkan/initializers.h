/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/descriptors.h>

#include <QRect>
#include <QVector>

namespace Qt3DRaytrace {
namespace Vulkan {

template<typename T>
struct Initializer : T
{
    Initializer(VkStructureType type) : T({type}) {}

    operator T*() { return static_cast<T*>(this); }
    operator const T*() const { return static_cast<const T*>(this); }
};

struct SamplerCreateInfo : Initializer<VkSamplerCreateInfo>
{
    SamplerCreateInfo(VkFilter filter_=VK_FILTER_NEAREST, VkSamplerMipmapMode mipmapMode_=VK_SAMPLER_MIPMAP_MODE_NEAREST)
        : Initializer(VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO)
    {
        magFilter = filter_;
        minFilter = filter_;
        mipmapMode = mipmapMode_;
    }
};

struct DescriptorPoolCreateInfo : Initializer<VkDescriptorPoolCreateInfo>
{
    DescriptorPoolCreateInfo(uint32_t maxSets_=0, VkDescriptorPoolCreateFlags flags_=0)
        : Initializer(VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO)
    {
        maxSets = maxSets_;
        flags = flags_;
    }
    DescriptorPoolCreateInfo(uint32_t maxSets_, const QVector<VkDescriptorPoolSize>& poolSizes_, VkDescriptorPoolCreateFlags flags_=0)
        : DescriptorPoolCreateInfo(maxSets_, flags_)
    {
        pPoolSizes = poolSizes_.data();
        poolSizeCount = uint32_t(poolSizes_.size());
    }
};

struct QueryPoolCreateInfo : Initializer<VkQueryPoolCreateInfo>
{
    QueryPoolCreateInfo(VkQueryType queryType_, uint32_t queryCount_, VkQueryPipelineStatisticFlags pipelineStatistics_=0)
        : Initializer(VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO)
    {
        queryType = queryType_;
        queryCount = queryCount_;
        pipelineStatistics = pipelineStatistics_;
    }
};

struct CommandPoolCreateInfo : Initializer<VkCommandPoolCreateInfo>
{
    CommandPoolCreateInfo(VkCommandPoolCreateFlags flags_, uint32_t queueFamilyIndex_)
        : Initializer(VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO)
    {
        flags = flags_;
        queueFamilyIndex = queueFamilyIndex_;
    }
};

struct ImageCreateInfo : Initializer<VkImageCreateInfo>
{
    ImageCreateInfo(VkImageType imageType_, VkFormat format_, const VkExtent3D &extent_, uint32_t mipLevels_=1, uint32_t arrayLayers_=1)
        : Initializer(VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO)
    {
        imageType = imageType_;
        format = format_;
        extent = extent_;
        mipLevels = mipLevels_;
        arrayLayers = arrayLayers_;

        samples = VK_SAMPLE_COUNT_1_BIT;
        tiling = VK_IMAGE_TILING_OPTIMAL;
        sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }
    ImageCreateInfo(VkImageType imageType_, VkFormat format_, const QSize &extent_, uint32_t mipLevels_=1, uint32_t arrayLayers_=1)
        : ImageCreateInfo(imageType_, format_, VkExtent3D{}, mipLevels_, arrayLayers_)
    {
        extent.width  = uint32_t(extent_.width());
        extent.height = uint32_t(extent_.height());
        extent.depth  = 1;
    }
};

struct ImageViewCreateInfo : Initializer<VkImageViewCreateInfo>
{
    ImageViewCreateInfo(VkImage image_, VkImageViewType viewType_, VkFormat format_, VkImageAspectFlags aspectMask_=VK_IMAGE_ASPECT_COLOR_BIT)
        : Initializer(VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO)
    {
        image = image_;
        viewType = viewType_;
        format = format_;
        subresourceRange.aspectMask = aspectMask_;
        subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
    }
};

struct BufferCreateInfo : Initializer<VkBufferCreateInfo>
{
    BufferCreateInfo(VkDeviceSize size_=0, VkBufferUsageFlags usage_=0)
        : Initializer(VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO)
    {
        size = size_;
        usage = usage_;
        sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
};

struct FramebufferCreateInfo : Initializer<VkFramebufferCreateInfo>
{
    FramebufferCreateInfo()
        : Initializer(VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO)
    {}
    FramebufferCreateInfo(VkRenderPass renderPass_, const QVector<VkImageView> &attachments_, uint32_t width_, uint32_t height_, uint32_t layers_=1)
        : FramebufferCreateInfo()
    {
        renderPass = renderPass_;
        attachmentCount = uint32_t(attachments_.size());
        pAttachments = attachments_.data();
        width = width_;
        height = height_;
        layers = layers_;
    }
};

struct RenderPassCreateInfo : Initializer<VkRenderPassCreateInfo>
{
    RenderPassCreateInfo()
        : Initializer(VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO)
    {}
};

struct AccelerationStructureCreateInfo : Initializer<VkAccelerationStructureCreateInfoNV>
{
    AccelerationStructureCreateInfo(const VkAccelerationStructureInfoNV &info_={}, VkDeviceSize compactedSize_=0)
        : Initializer(VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV)
    {
        info = info_;
        compactedSize = compactedSize_;
    }
};

struct CommandBufferAllocateInfo : Initializer<VkCommandBufferAllocateInfo>
{
    CommandBufferAllocateInfo(VkCommandPool commandPool_, VkCommandBufferLevel level_, uint32_t count_=1)
        : Initializer(VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO)
    {
        commandPool = commandPool_;
        level = level_;
        commandBufferCount = count_;
    }
};

struct DescriptorSetAllocateInfo : Initializer<VkDescriptorSetAllocateInfo>
{
    DescriptorSetAllocateInfo(VkDescriptorPool descriptorPool_)
        : Initializer(VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO)
    {
        descriptorPool = descriptorPool_;
    }
    DescriptorSetAllocateInfo(VkDescriptorPool descriptorPool_, const QVector<VkDescriptorSetLayout> &setLayouts_)
        : DescriptorSetAllocateInfo(descriptorPool_)
    {
        pSetLayouts = setLayouts_.data();
        descriptorSetCount = uint32_t(setLayouts_.size());
    }
};

struct BufferMemoryBarrier : Initializer<VkBufferMemoryBarrier>
{
    BufferMemoryBarrier(VkAccessFlags srcAccessMask_, VkAccessFlags dstAccessMask_, VkBuffer buffer_, const BufferRange &range_={})
        : Initializer(VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER)
    {
        srcAccessMask = srcAccessMask_;
        dstAccessMask = dstAccessMask_;
        buffer = buffer_;
        offset = range_.offset;
        size = range_.size;
        srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    }
};

struct ImageMemoryBarrier : Initializer<VkImageMemoryBarrier>
{
    ImageMemoryBarrier(VkAccessFlags srcAccessMask_, VkAccessFlags dstAccessMask_, VkImageLayout oldLayout_, VkImageLayout newLayout_, VkImage image_, const ImageSubresourceRange &subresourceRange_)
        : Initializer(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER)
    {
        srcAccessMask = srcAccessMask_;
        dstAccessMask = dstAccessMask_;
        oldLayout = oldLayout_;
        newLayout = newLayout_;
        image = image_;
        subresourceRange = subresourceRange_;
        srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    }
};

struct WriteDescriptorSet : Initializer<VkWriteDescriptorSet>
{
    WriteDescriptorSet(VkDescriptorSet dstSet_, uint32_t dstBinding_, uint32_t dstArrayElement_, VkDescriptorType descriptorType_, uint32_t descriptorCount_=1)
        : Initializer(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET)
    {
        dstSet = dstSet_;
        dstBinding = dstBinding_;
        dstArrayElement = dstArrayElement_;
        descriptorType = descriptorType_;
        descriptorCount = descriptorCount_;
    }
    WriteDescriptorSet(VkDescriptorSet dstSet_, uint32_t dstBinding_, uint32_t dstArrayElement_, VkDescriptorType descriptorType_, const DescriptorImageInfo &imageInfo_)
        : WriteDescriptorSet(dstSet_, dstBinding_, dstArrayElement_, descriptorType_)
    {
        pImageInfo = &imageInfo_;
    }
    WriteDescriptorSet(VkDescriptorSet dstSet_, uint32_t dstBinding_, uint32_t dstArrayElement_, VkDescriptorType descriptorType_, const QVector<DescriptorImageInfo> &imageInfos_)
        : WriteDescriptorSet(dstSet_, dstBinding_, dstArrayElement_, descriptorType_, uint32_t(imageInfos_.size()))
    {
        pImageInfo = imageInfos_.data();
    }
    WriteDescriptorSet(VkDescriptorSet dstSet_, uint32_t dstBinding_, uint32_t dstArrayElement_, VkDescriptorType descriptorType_, const DescriptorBufferInfo &bufferInfo_)
        : WriteDescriptorSet(dstSet_, dstBinding_, dstArrayElement_, descriptorType_)
    {
        pBufferInfo = &bufferInfo_;
    }
    WriteDescriptorSet(VkDescriptorSet dstSet_, uint32_t dstBinding_, uint32_t dstArrayElement_, VkDescriptorType descriptorType_, const QVector<DescriptorBufferInfo> &bufferInfos_)
        : WriteDescriptorSet(dstSet_, dstBinding_, dstArrayElement_, descriptorType_, uint32_t(bufferInfos_.size()))
    {
        pBufferInfo = bufferInfos_.data();
    }
    WriteDescriptorSet(VkDescriptorSet dstSet_, uint32_t dstBinding_, uint32_t dstArrayElement_, VkDescriptorType descriptorType_, VkBufferView texelBufferView_)
        : WriteDescriptorSet(dstSet_, dstBinding_, dstArrayElement_, descriptorType_)
    {
        pTexelBufferView = &texelBufferView_;
    }
    WriteDescriptorSet(VkDescriptorSet dstSet_, uint32_t dstBinding_, uint32_t dstArrayElement_, VkDescriptorType descriptorType_, const QVector<VkBufferView> &texelBufferViews_)
        : WriteDescriptorSet(dstSet_, dstBinding_, dstArrayElement_, descriptorType_, uint32_t(texelBufferViews_.size()))
    {
        pTexelBufferView = texelBufferViews_.data();
    }
};

struct RenderPassBeginInfo : Initializer<VkRenderPassBeginInfo>
{
    RenderPassBeginInfo(VkRenderPass renderPass_=VK_NULL_HANDLE, VkFramebuffer framebuffer_=VK_NULL_HANDLE, const QRect &renderArea_={})
        : Initializer(VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO)
    {
        renderPass = renderPass_;
        framebuffer = framebuffer_;
        renderArea.offset.x = int32_t(renderArea_.x());
        renderArea.offset.y = int32_t(renderArea_.y());
        renderArea.extent.width = uint32_t(renderArea_.width());
        renderArea.extent.height = uint32_t(renderArea_.height());
    }
};

struct AllocationCreateInfo : VmaAllocationCreateInfo
{
    AllocationCreateInfo(VmaMemoryUsage usage_, VkMemoryPropertyFlags requiredFlags_=0, VkMemoryPropertyFlags preferredFlags_=0)
        : VmaAllocationCreateInfo({})
    {
        usage = usage_;
        requiredFlags = requiredFlags_;
        preferredFlags = preferredFlags_;
        flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }
    operator const VmaAllocationCreateInfo*() const
    {
        return this;
    }
};

} // Vulkan
} // Qt3DRaytrace
