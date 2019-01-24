/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <cstdint>

#include <volk.h>
#include <vk_mem_alloc.h>

#include <QLoggingCategory>

#define VKSUCCEEDED(x) ((x) == VK_SUCCESS)
#define VKFAILED(x)    ((x) != VK_SUCCESS)

namespace Qt3DRaytrace {
namespace Vulkan {

Q_DECLARE_LOGGING_CATEGORY(logVulkan)

class Device;

class Result
{
public:
    Result(VkResult r = VK_SUCCESS)
        : m_result(r)
    {}

    operator VkResult() const { return m_result; }
    operator bool() const { return m_result == VK_SUCCESS; }

    const char *toString() const
    {
#define VK_RESULT_STRING(result) case result: return #result
        switch(m_result) {
        VK_RESULT_STRING(VK_SUCCESS);
        VK_RESULT_STRING(VK_NOT_READY);
        VK_RESULT_STRING(VK_TIMEOUT);
        VK_RESULT_STRING(VK_EVENT_SET);
        VK_RESULT_STRING(VK_EVENT_RESET);
        VK_RESULT_STRING(VK_INCOMPLETE);
        VK_RESULT_STRING(VK_ERROR_OUT_OF_HOST_MEMORY);
        VK_RESULT_STRING(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        VK_RESULT_STRING(VK_ERROR_INITIALIZATION_FAILED);
        VK_RESULT_STRING(VK_ERROR_DEVICE_LOST);
        VK_RESULT_STRING(VK_ERROR_MEMORY_MAP_FAILED);
        VK_RESULT_STRING(VK_ERROR_LAYER_NOT_PRESENT);
        VK_RESULT_STRING(VK_ERROR_EXTENSION_NOT_PRESENT);
        VK_RESULT_STRING(VK_ERROR_FEATURE_NOT_PRESENT);
        VK_RESULT_STRING(VK_ERROR_INCOMPATIBLE_DRIVER);
        VK_RESULT_STRING(VK_ERROR_TOO_MANY_OBJECTS);
        VK_RESULT_STRING(VK_ERROR_FORMAT_NOT_SUPPORTED);
        VK_RESULT_STRING(VK_ERROR_FRAGMENTED_POOL);
        VK_RESULT_STRING(VK_ERROR_OUT_OF_POOL_MEMORY);
        VK_RESULT_STRING(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        VK_RESULT_STRING(VK_ERROR_SURFACE_LOST_KHR);
        VK_RESULT_STRING(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        VK_RESULT_STRING(VK_SUBOPTIMAL_KHR);
        VK_RESULT_STRING(VK_ERROR_OUT_OF_DATE_KHR);
        VK_RESULT_STRING(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        VK_RESULT_STRING(VK_ERROR_VALIDATION_FAILED_EXT);
        VK_RESULT_STRING(VK_ERROR_INVALID_SHADER_NV);
        VK_RESULT_STRING(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        VK_RESULT_STRING(VK_ERROR_FRAGMENTATION_EXT);
        VK_RESULT_STRING(VK_ERROR_NOT_PERMITTED_EXT);
        default:
            return "VK_UNKNOWN_ERROR";
        }
#undef VK_RESULT_STRING
    }

private:
    VkResult m_result;
};

template<typename T>
struct Resource
{
    Resource(T handle = VK_NULL_HANDLE)
        : handle(handle)
    {}

    operator T() const { return handle; }
    operator T*() { return &handle; }
    operator const T*() const { return &handle; }
    operator bool() const { return isValid(); }

    bool isValid() const
    {
        return handle != VK_NULL_HANDLE;
    }

    T handle;
};

template<typename T>
struct MemoryResource : Resource<T>
{
    MemoryResource(T handle = VK_NULL_HANDLE)
        : Resource<T>(handle)
        , allocation(VK_NULL_HANDLE)
    {}

    bool isAllocated() const
    {
        return allocation != VK_NULL_HANDLE;
    }

    VmaAllocation allocation;
};

struct BufferRange
{
    VkDeviceSize offset = 0;
    VkDeviceSize size = VK_WHOLE_SIZE;
};

struct MipLevelRange
{
    uint32_t baseMipLevel = 0;
    uint32_t levelCount = VK_REMAINING_MIP_LEVELS;
};

struct ArrayLayerRange
{
    uint32_t baseArrayLayer = 0;
    uint32_t layerCount = VK_REMAINING_ARRAY_LAYERS;
};

struct ImageSubresourceRange : VkImageSubresourceRange
{
    ImageSubresourceRange(VkImageAspectFlags aspectMask_=VK_IMAGE_ASPECT_COLOR_BIT, const MipLevelRange &mipLevelRange={}, const ArrayLayerRange &arrayLayerRange={})
    {
        aspectMask = aspectMask_;
        baseMipLevel = mipLevelRange.baseMipLevel;
        levelCount = mipLevelRange.levelCount;
        baseArrayLayer = arrayLayerRange.baseArrayLayer;
        layerCount = arrayLayerRange.layerCount;
    }
    ImageSubresourceRange(VkImageAspectFlags aspectMask_, const ArrayLayerRange &arrayLayerRange)
        : ImageSubresourceRange(aspectMask_, MipLevelRange{}, arrayLayerRange)
    {}
};

} // Vulkan
} // Qt3DRaytrace
