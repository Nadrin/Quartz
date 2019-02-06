/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/vkresources.h>
#include <renderers/vulkan/descriptors.h>

#include <QMap>
#include <QAtomicInteger>

namespace Qt3DRaytrace {
namespace Vulkan {

class Renderer;
class Device;

class DescriptorManager
{
public:
    explicit DescriptorManager(Renderer *renderer);
    ~DescriptorManager();

    bool createDescriptorPool(ResourceClass rclass, uint32_t capacity);
    void destroyDescriptorPool(ResourceClass rclass);
    void destroyAllDescriptorPools();

    DescriptorHandle allocateDescriptor(ResourceClass rclass);
    void updateBufferDescriptor(DescriptorHandle handle, const DescriptorBufferInfo &bufferInfo) const;

    VkDescriptorSet descriptorSet(ResourceClass rclass) const;
    uint32_t descriptorPoolCapacity(ResourceClass rclass) const;
    VkDescriptorBindingFlagsEXT descriptorBindingFlags(ResourceClass rclass) const;

    Q_DISABLE_COPY(DescriptorManager)

private:
    Device *m_device;

    struct DescriptorPoolInfo {
        VkDescriptorPool pool;
        VkDescriptorSet set;
        VkDescriptorSetLayout layout;
        QAtomicInteger<uint32_t> allocated;
        uint32_t capacity;
    };
    QMap<ResourceClass, DescriptorPoolInfo> m_pools;
};

} // Vulkan
} // Qt3DRaytrace
