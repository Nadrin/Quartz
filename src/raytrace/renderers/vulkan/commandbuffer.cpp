/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/commandbuffer.h>
#include <renderers/vulkan/device.h>

namespace Qt3DRaytrace {
namespace Vulkan {

CommandBuffer::CommandBuffer(VkCommandBuffer commandBuffer)
{
    handle = commandBuffer;
}

Result CommandBuffer::begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo *inheritanceInfo) const
{
    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = flags;
    beginInfo.pInheritanceInfo = inheritanceInfo;

    Result result;
    if(VKFAILED(result = vkBeginCommandBuffer(handle, &beginInfo))) {
        qCCritical(logVulkan) << "Begin command buffer failed:" << result.toString();
    }
    return result;
}

Result CommandBuffer::end() const
{
    Result result;
    if(VKFAILED(result = vkEndCommandBuffer(handle))) {
        qCCritical(logVulkan) << "End command buffer failed:" << result.toString();
    }
    return result;
}

Result CommandBuffer::reset(VkCommandBufferResetFlags flags) const
{
    Result result;
    if(VKFAILED(result = vkResetCommandBuffer(handle, flags))) {
        qCCritical(logVulkan) << "Reset command buffer failed:" << result.toString();
    }
    return result;
}

void CommandBuffer::resourceBarrier(int numBufferTransitions, const BufferTransition *bufferTransitions, int numImageTransitions, const ImageTransition *imageTransitions) const
{
    VkPipelineStageFlags srcStageMask = 0;
    VkPipelineStageFlags dstStageMask = 0;

    QVarLengthArray<VkBufferMemoryBarrier, 32> bufferBarriers;
    for(int i=0; i<numBufferTransitions; ++i) {
        const BufferTransition &transition = bufferTransitions[i];
        const int oldStateIndex = static_cast<int>(transition.oldState);
        const int newStateIndex = static_cast<int>(transition.newState);

        VkAccessFlags srcAccessMask = ResourceBarrier::BufferStateToAccessMask[oldStateIndex];
        VkAccessFlags dstAccessMask = ResourceBarrier::BufferStateToAccessMask[newStateIndex];

        srcStageMask |= ResourceBarrier::BufferStateToStageFlags[oldStateIndex];
        dstStageMask |= ResourceBarrier::BufferStateToStageFlags[newStateIndex];
        bufferBarriers.append(BufferMemoryBarrier(srcAccessMask, dstAccessMask, transition.buffer, transition.range));
    }

    QVarLengthArray<VkImageMemoryBarrier, 32> imageBarriers;
    for(int i=0; i<numImageTransitions; ++i) {
        const ImageTransition &transition = imageTransitions[i];
        const int oldStateIndex = static_cast<int>(transition.oldState);
        const int newStateIndex = static_cast<int>(transition.newState);

        VkAccessFlags srcAccessMask = ResourceBarrier::ImageStateToAccessMask[oldStateIndex];
        VkAccessFlags dstAccessMask = ResourceBarrier::ImageStateToAccessMask[newStateIndex];
        VkImageLayout oldLayout = ResourceBarrier::ImageStateToImageLayout[oldStateIndex];
        VkImageLayout newLayout = ResourceBarrier::ImageStateToImageLayout[newStateIndex];

        srcStageMask |= ResourceBarrier::ImageStateToStageFlags[oldStateIndex];
        dstStageMask |= ResourceBarrier::ImageStateToStageFlags[newStateIndex];
        imageBarriers.append(ImageMemoryBarrier(srcAccessMask, dstAccessMask, oldLayout, newLayout, transition.image, transition.range));
    }

    uint32_t numBufferMemoryBarriers = uint32_t(bufferBarriers.size());
    const VkBufferMemoryBarrier *bufferMemoryBarriers = (numBufferMemoryBarriers > 0) ? bufferBarriers.data() : nullptr;
    uint32_t numImageMemoryBarriers = uint32_t(imageBarriers.size());
    const VkImageMemoryBarrier *imageMemoryBarriers = (numImageMemoryBarriers > 0) ? imageBarriers.data() : nullptr;

    if(srcStageMask == 0) {
        srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    if(dstStageMask == 0) {
        dstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }

    // TODO: Handle VK_DEPENDENCY_BY_REGION_BIT
    VkDependencyFlags dependencyFlags = 0;
    vkCmdPipelineBarrier(handle, srcStageMask, dstStageMask, dependencyFlags,
                         0, nullptr, numBufferMemoryBarriers, bufferMemoryBarriers, numImageMemoryBarriers, imageMemoryBarriers);
}

void CommandBuffer::resourceBarrier(const QVector<BufferTransition> &bufferTransitions, const QVector<ImageTransition> &imageTransitions) const
{
    const BufferTransition *bufferTransitionsData = bufferTransitions.size() > 0 ? bufferTransitions.data() : nullptr;
    const ImageTransition *imageTransitionsData = imageTransitions.size() > 0 ? imageTransitions.data() : nullptr;
    resourceBarrier(bufferTransitions.size(), bufferTransitionsData, imageTransitions.size(), imageTransitionsData);
}

void CommandBuffer::resourceBarrier(const QVector<BufferTransition> &bufferTransitions) const
{
    const BufferTransition *bufferTransitionsData = bufferTransitions.size() > 0 ? bufferTransitions.data() : nullptr;
    resourceBarrier(bufferTransitions.size(), bufferTransitionsData, 0, nullptr);
}

void CommandBuffer::resourceBarrier(const QVector<ImageTransition> &imageTransitions) const
{
    const ImageTransition *imageTransitionsData = imageTransitions.size() > 0 ? imageTransitions.data() : nullptr;
    resourceBarrier(0, nullptr, imageTransitions.size(), imageTransitionsData);
}

} // Vulkan
} // Qt3DRaytrace
