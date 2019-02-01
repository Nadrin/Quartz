/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/managers/descriptormanager.h>
#include <renderers/vulkan/device.h>

#include <QMutexLocker>

namespace Qt3DRaytrace {
namespace Vulkan {

static VkDescriptorType resourceClassToDescriptorType(ResourceClass rclass)
{
    switch(rclass) {
    case ResourceClass::AttributeBuffer:
    case ResourceClass::IndexBuffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    }
}

DescriptorManager::DescriptorManager(Device *device)
    : m_device(device)
{
    Q_ASSERT(m_device);
}

DescriptorManager::~DescriptorManager()
{
    destroyAllDescriptorPools();
}

bool DescriptorManager::createDescriptorPool(ResourceClass rclass, uint32_t capacity)
{
    Q_ASSERT(capacity > 0);
    Q_ASSERT(!m_pools.contains(rclass));

    DescriptorPoolInfo poolInfo;
    poolInfo.allocated = 0;
    poolInfo.capacity = capacity;

    Result result;

    const VkDescriptorType type = resourceClassToDescriptorType(rclass);

    VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL;
    if(type == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT) {
        stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    const VkDescriptorBindingFlagsEXT bindingFlags = descriptorBindingFlags(rclass);

    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = 0;
    layoutBinding.descriptorType = type;
    layoutBinding.descriptorCount = capacity;
    layoutBinding.stageFlags = stageFlags;

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT layoutBindingFlags = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT };
    layoutBindingFlags.bindingCount = 1;
    layoutBindingFlags.pBindingFlags = &bindingFlags;

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    layoutCreateInfo.pNext = &layoutBindingFlags;
    layoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
    layoutCreateInfo.bindingCount = 1;
    layoutCreateInfo.pBindings = &layoutBinding;
    if(VKFAILED(result = vkCreateDescriptorSetLayout(*m_device, &layoutCreateInfo, nullptr, &poolInfo.layout))) {
        qCCritical(logVulkan) << "DescriptorManager: Failed to create descriptor set layout for descriptor pool:" << result.toString();
        return false;
    }

    VkDescriptorPoolSize poolSize;
    poolSize.type = type;
    poolSize.descriptorCount = capacity;

    VkDescriptorPoolCreateInfo poolCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
    poolCreateInfo.maxSets = 1;
    poolCreateInfo.poolSizeCount = 1;
    poolCreateInfo.pPoolSizes = &poolSize;
    if(VKFAILED(result = vkCreateDescriptorPool(*m_device, &poolCreateInfo, nullptr, &poolInfo.pool))) {
        qCCritical(logVulkan) << "DescriptorManager: Failed to create descriptor pool:" << result.toString();
        vkDestroyDescriptorSetLayout(*m_device, poolInfo.layout, nullptr);
        return false;
    }

    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT variableCountAllocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT };
    variableCountAllocInfo.descriptorSetCount = 1;
    variableCountAllocInfo.pDescriptorCounts = &capacity;

    VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    allocInfo.pNext = &variableCountAllocInfo;
    allocInfo.descriptorPool = poolInfo.pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &poolInfo.layout;
    if(VKFAILED(result = vkAllocateDescriptorSets(*m_device, &allocInfo, &poolInfo.set))) {
        qCCritical(logVulkan) << "DescriptorManager: Failed to allocate descriptor set for descriptor pool:" << result.toString();
        vkDestroyDescriptorPool(*m_device, poolInfo.pool, nullptr);
        vkDestroyDescriptorSetLayout(*m_device, poolInfo.layout, nullptr);
        return false;
    }

    m_pools.insert(rclass, poolInfo);
    return true;
}

void DescriptorManager::destroyDescriptorPool(ResourceClass rclass)
{
    DescriptorPoolInfo poolInfo;
    {
        auto it = m_pools.find(rclass);
        if(it == m_pools.end()) {
            return;
        }
        poolInfo = *it;
        m_pools.erase(it);
    }

    vkDestroyDescriptorPool(*m_device, poolInfo.pool, nullptr);
    vkDestroyDescriptorSetLayout(*m_device, poolInfo.layout, nullptr);
}

void DescriptorManager::destroyAllDescriptorPools()
{
    for(const DescriptorPoolInfo &poolInfo : m_pools) {
        vkDestroyDescriptorPool(*m_device, poolInfo.pool, nullptr);
        vkDestroyDescriptorSetLayout(*m_device, poolInfo.layout, nullptr);
    }
    m_pools.clear();
}

DescriptorHandle DescriptorManager::allocateDescriptor(ResourceClass rclass)
{
    Q_ASSERT(m_pools.contains(rclass));

    auto &pool = m_pools[rclass];
    uint32_t descriptorIndex = pool.allocated.fetchAndAddAcquire(1);
    if(descriptorIndex >= pool.capacity) {
        // TODO: Implement exponential pool grow via re-allocation.
        Q_ASSERT_X(false, Q_FUNC_INFO, "Descriptor pool must not be full");
    }
    return DescriptorHandle{ descriptorIndex + 1, rclass };
}

void DescriptorManager::updateBufferDescriptor(DescriptorHandle handle, const DescriptorBufferInfo &bufferInfo) const
{
    Q_ASSERT(handle > 0);
    Q_ASSERT(m_pools.contains(handle.rclass));

    const auto &pool = m_pools[handle.rclass];
    uint32_t descriptorIndex = handle.index - 1;
    Q_ASSERT(descriptorIndex < pool.capacity);
    m_device->writeDescriptor({pool.set, 0, descriptorIndex, resourceClassToDescriptorType(handle.rclass), bufferInfo});
}

VkDescriptorSet DescriptorManager::descriptorSet(ResourceClass rclass) const
{
    Q_ASSERT(m_pools.contains(rclass));
    return m_pools[rclass].set;
}

uint32_t DescriptorManager::descriptorPoolCapacity(ResourceClass rclass) const
{
    auto it = m_pools.find(rclass);
    return (it != m_pools.end()) ? it->capacity : 0;
}

VkDescriptorBindingFlagsEXT DescriptorManager::descriptorBindingFlags(ResourceClass rclass) const
{
    Q_UNUSED(rclass);
    return VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT |
           VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT |
           VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT |
           VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT;
}

} // Vulkan
} // Qt3DRaytrace
