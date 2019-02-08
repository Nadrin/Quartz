/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/vkresources.h>
#include <renderers/vulkan/descriptors.h>
#include <renderers/vulkan/shadermodule.h>

#include <initializer_list>

#include <QVector>
#include <QMap>
#include <QPair>

namespace Qt3DRaytrace {
namespace Vulkan {

class DescriptorManager;

struct Pipeline : Resource<VkPipeline>
{
    VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    QVector<VkDescriptorSetLayout> descriptorSetLayouts;
    QVector<VkPushConstantRange> pushConstantRanges;
};

class PipelineBuilder
{
public:
    ~PipelineBuilder();

    PipelineBuilder &shaders(const QVector<const ShaderModule*> &modules);
    PipelineBuilder &shaders(std::initializer_list<QString> moduleNames);
    PipelineBuilder &bytecodes(std::initializer_list<QByteArray> moduleBytecodes);

    PipelineBuilder &defaultSampler(VkSampler sampler);
    PipelineBuilder &sampler(uint32_t set, uint32_t binding, VkSampler sampler);
    PipelineBuilder &sampler(const QString &name, VkSampler sampler);

    PipelineBuilder &descriptorSetLayoutFlags(uint32_t set, VkDescriptorSetLayoutCreateFlags flags);

    PipelineBuilder &descriptorBindingFlags(uint32_t set, uint32_t binding, VkDescriptorBindingFlagsEXT flags);
    PipelineBuilder &descriptorBindingFlags(const QString &name, VkDescriptorBindingFlagsEXT flags);
    PipelineBuilder &descriptorBindingCount(uint32_t set, uint32_t binding, uint32_t count);
    PipelineBuilder &descriptorBindingCount(const QString &name, uint32_t count);
    PipelineBuilder &descriptorBindingManager(uint32_t set, uint32_t binding, const DescriptorManager *manager, ResourceClass rclass);
    PipelineBuilder &descriptorBindingManager(const QString &name, const DescriptorManager *manager, ResourceClass rclass);

protected:
    explicit PipelineBuilder(Device *device);

    bool buildBasePipeline(Pipeline &pipeline) const;

    Device *m_device;
    Sampler m_defaultSampler;

    QVector<const ShaderModule*> m_shaders;

    using BindingID = QPair<uint32_t, uint32_t>;
    QMap<BindingID, VkSampler> m_samplersByID;
    QMap<QString, VkSampler> m_samplersByName;

    struct DescriptorBindingInfo {
        VkDescriptorBindingFlagsEXT flags = 0;
        VkShaderStageFlags stageFlags = 0;
        uint32_t count = 0;
    };
    QMap<BindingID, DescriptorBindingInfo> m_descriptorBindingInfoByID;
    QMap<QString, DescriptorBindingInfo> m_descriptorBindingInfoByName;

    QMap<uint32_t, VkDescriptorSetLayoutCreateFlags> m_descriptorSetLayoutFlags;

    bool m_allowDuplicateShaderStages = false;

private:
    QVector<VkDescriptorSetLayout> buildDescriptorSetLayouts() const;
    QVector<VkPushConstantRange> buildPushConstantRanges() const;

    QVector<ShaderModule*> m_ownedModules;
    bool m_needsDescriptorBindingFlags;
};

template<typename T>
class PipelineBuilderImpl : public PipelineBuilder
{
public:
    T &shaders(const QVector<const ShaderModule*> &modules)
    {
        return static_cast<T&>(PipelineBuilder::shaders(modules));
    }
    T &shaders(std::initializer_list<QString> moduleNames)
    {
        return static_cast<T&>(PipelineBuilder::shaders(moduleNames));
    }
    T &bytecodes(std::initializer_list<QByteArray> moduleBytecodes)
    {
        return static_cast<T&>(PipelineBuilder::bytecodes(moduleBytecodes));
    }
    T &defaultSampler(VkSampler sampler)
    {
        return static_cast<T&>(PipelineBuilder::defaultSampler(sampler));
    }
    T &sampler(uint32_t set, uint32_t binding, VkSampler sampler)
    {
        return static_cast<T&>(PipelineBuilder::sampler(set, binding, sampler));
    }
    T &sampler(const QString &name, VkSampler sampler)
    {
        return static_cast<T&>(PipelineBuilder::sampler(name, sampler));
    }
    T &descriptorSetLayoutFlags(uint32_t set, VkDescriptorSetLayoutCreateFlags flags)
    {
        return static_cast<T&>(PipelineBuilder::descriptorSetLayoutFlags(set, flags));
    }
    T &descriptorBindingFlags(uint32_t set, uint32_t binding, VkDescriptorBindingFlagsEXT flags)
    {
        return static_cast<T&>(PipelineBuilder::descriptorBindingFlags(set, binding, flags));
    }
    T &descriptorBindingFlags(const QString &name, VkDescriptorBindingFlagsEXT flags)
    {
        return static_cast<T&>(PipelineBuilder::descriptorBindingFlags(name, flags));
    }
    T &descriptorBindingCount(uint32_t set, uint32_t binding, uint32_t count)
    {
        return static_cast<T&>(PipelineBuilder::descriptorBindingCount(set, binding, count));
    }
    T &descriptorBindingCount(const QString &name, uint32_t count)
    {
        return static_cast<T&>(PipelineBuilder::descriptorBindingCount(name, count));
    }
    T &descriptorBindingManager(uint32_t set, uint32_t binding, DescriptorManager *manager, ResourceClass rclass)
    {
        return static_cast<T&>(PipelineBuilder::descriptorBindingManager(set, binding, manager, rclass));
    }
    T &descriptorBindingManager(const QString &name, DescriptorManager *manager, ResourceClass rclass)
    {
        return static_cast<T&>(PipelineBuilder::descriptorBindingManager(name, manager, rclass));
    }

protected:
    explicit PipelineBuilderImpl(Device *device)
        : PipelineBuilder(device)
    {}
};

} // Vulkan
} // Qt3DRaytrace
