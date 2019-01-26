/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/vkresources.h>
#include <renderers/vulkan/shadermodule.h>

#include <initializer_list>

#include <QVector>
#include <QMap>
#include <QPair>

namespace Qt3DRaytrace {
namespace Vulkan {

class Pipeline : public Resource<VkPipeline>
{
public:
    explicit Pipeline(VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);

    VkPipelineBindPoint bindPoint() const { return m_bindPoint; }

    VkPipelineLayout pipelineLayout;
    QVector<VkDescriptorSetLayout> descriptorSetLayouts;

private:
    VkPipelineBindPoint m_bindPoint;
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

protected:
    explicit PipelineBuilder(Device *device);

    QVector<VkDescriptorSetLayout> buildDescriptorSetLayouts() const;
    VkPipelineLayout buildPipelineLayout(const QVector<VkDescriptorSetLayout> &descriptorSetLayouts) const;

    VkDevice m_device;
    VkSampler m_defaultSampler;

    QVector<const ShaderModule*> m_shaders;

    using BindingID = QPair<uint32_t, uint32_t>;
    QMap<BindingID, VkSampler> m_samplersByID;
    QMap<QString, VkSampler> m_samplersByName;

private:
    QVector<ShaderModule*> m_ownedModules;
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

protected:
    explicit PipelineBuilderImpl(Device *device)
        : PipelineBuilder(device)
    {}
};

} // Vulkan
} // Qt3DRaytrace
