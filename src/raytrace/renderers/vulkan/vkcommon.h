/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <volk.h>
#include <QLoggingCategory>

#define VKFAILED(x) ((x) != VK_SUCCESS)

namespace Qt3DRaytrace {

Q_DECLARE_LOGGING_CATEGORY(logVulkan)

namespace Vulkan {

template<typename T>
class DeviceHandle
{
public:
    VkDevice device() const { return m_device; }
    T handle() const { return m_handle; }

    operator T() const { return m_handle; }
    operator bool() const { return isValid(); }

    bool isValid() const
    {
        return m_device != VK_NULL_HANDLE && m_handle != VK_NULL_HANDLE;
    }

protected:
    explicit DeviceHandle(VkDevice device=VK_NULL_HANDLE, T handle=VK_NULL_HANDLE)
        : m_device(device)
        , m_handle(handle)
    {}

    void reset()
    {
        m_device = VK_NULL_HANDLE;
        m_handle = VK_NULL_HANDLE;
    }

    VkDevice m_device;
    T m_handle;
};

} // Vulkan
} // Qt3DRaytrace
