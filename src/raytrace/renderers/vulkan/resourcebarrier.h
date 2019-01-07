/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

// D3D12 style resource barriers on top of vkCmdPipelineBarrier.

#include <renderers/vulkan/vkcommon.h>

#include <QVector>

namespace Qt3DRaytrace {
namespace Vulkan {

enum class BufferState
{
    Undefined = 0,
    VertexAttribRead,
    IndexRead,
    UniformRead,
    ShaderRead,
    ShaderReadWrite,
    CopySource,
    CopyDest,
    NumBufferStates,
};

struct BufferTransition
{
    VkBuffer buffer = VK_NULL_HANDLE;
    BufferState oldState = BufferState::Undefined;
    BufferState newState = BufferState::Undefined;
    BufferRange range;
};

enum class ImageState
{
    Undefined = 0,
    ColorAttachment,
    DepthStencilAttachment,
    DepthStencilReadWrite,
    ShaderRead,
    ShaderReadWrite,
    CopySource,
    CopyDest,
    PresentSource,
    NumImageStates,
};

struct ImageTransition
{
    VkImage image = VK_NULL_HANDLE;
    ImageState oldState = ImageState::Undefined;
    ImageState newState = ImageState::Undefined;
    ImageSubresourceRange range;
};

constexpr VkPipelineStageFlags VK_PIPELINE_STAGE_ALL_SHADERS =
        VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
        //VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
        //VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
        //VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
        //VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV |
        //VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV |
        VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV;

namespace ResourceBarrier
{
    static constexpr int NumBufferStates = static_cast<int>(BufferState::NumBufferStates);
    static constexpr VkPipelineStageFlags BufferStateToStageFlags[NumBufferStates] = {
        0, // Undefined
        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, // VertexAttribRead
        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, // IndexRead
        VK_PIPELINE_STAGE_ALL_SHADERS, // UniformRead
        VK_PIPELINE_STAGE_ALL_SHADERS, // ShaderRead
        VK_PIPELINE_STAGE_ALL_SHADERS, // ShaderReadWrite
        VK_PIPELINE_STAGE_TRANSFER_BIT, // CopySource
        VK_PIPELINE_STAGE_TRANSFER_BIT, // CopyDest
    };
    static constexpr VkAccessFlags BufferStateToAccessMask[NumBufferStates] = {
        0, // Undefined
        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, // VertexAttribRead
        VK_ACCESS_INDEX_READ_BIT, // IndexRead
        VK_ACCESS_UNIFORM_READ_BIT, // UniformRead
        VK_ACCESS_SHADER_READ_BIT, // ShaderRead
        VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, // ShaderReadWrite
        VK_ACCESS_TRANSFER_READ_BIT, // CopySource
        VK_ACCESS_TRANSFER_WRITE_BIT, // CopyDest
    };

    static constexpr int NumImageStates = static_cast<int>(ImageState::NumImageStates);
    static constexpr VkPipelineStageFlags ImageStateToStageFlags[NumImageStates] = {
        0, // Undefined
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // ColorAttachment
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, // DepthStencilAttachment
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, // DepthStencilReadWrite
        VK_PIPELINE_STAGE_ALL_SHADERS, // ShaderRead
        VK_PIPELINE_STAGE_ALL_SHADERS, // ShaderReadWrite
        VK_PIPELINE_STAGE_TRANSFER_BIT, // CopySource
        VK_PIPELINE_STAGE_TRANSFER_BIT, // CopyDest
        0, // Present
    };
    static constexpr VkAccessFlags ImageStateToAccessMask[NumImageStates] = {
        0, // Undefined
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // ColorAttachment
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT, // DepthStencilAttachment
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, // DepthStencilReadWrite
        VK_ACCESS_SHADER_READ_BIT, // ShaderRead
        VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, // ShaderReadWrite
        VK_ACCESS_TRANSFER_READ_BIT, // CopySource
        VK_ACCESS_TRANSFER_WRITE_BIT, // CopyDest
        VK_ACCESS_MEMORY_READ_BIT, // PresentSource
    };
    static constexpr VkImageLayout ImageStateToImageLayout[NumImageStates] = {
        VK_IMAGE_LAYOUT_UNDEFINED, // Undefined
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // ColorAttachment
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, // DepthStencilAttachment
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, // DepthStencilReadWrite
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, // ShaderRead
        VK_IMAGE_LAYOUT_GENERAL, // ShaderReadWrite
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, // CopySource
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, // CopyDest
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, // PresentSource
    };

    static constexpr VkImageLayout getImageLayoutFromState(ImageState state)
    {
        return ImageStateToImageLayout[static_cast<int>(state)];
    }
} // ResourceBarrier

} // Vulkan
} // Qt3DRaytrace
