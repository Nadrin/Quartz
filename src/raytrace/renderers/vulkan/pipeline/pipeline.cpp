/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/pipeline/pipeline.h>
#include <renderers/vulkan/device.h>
#include <renderers/vulkan/managers/descriptormanager.h>

namespace Qt3DRaytrace {
namespace Vulkan {

PipelineBuilder::PipelineBuilder(Device *device)
    : m_device(device)
    , m_defaultSampler(VK_NULL_HANDLE)
    , m_needsDescriptorBindingFlags(false)
{
    Q_ASSERT(m_device);
}

PipelineBuilder::~PipelineBuilder()
{
    qDeleteAll(m_ownedModules);
}

bool PipelineBuilder::buildBasePipeline(Pipeline &pipeline) const
{
    pipeline.descriptorSetLayouts = buildDescriptorSetLayouts();
    pipeline.pushConstantRanges = buildPushConstantRanges();

    VkPipelineLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    createInfo.setLayoutCount = uint32_t(pipeline.descriptorSetLayouts.size());
    if(createInfo.setLayoutCount > 0) {
        createInfo.pSetLayouts = pipeline.descriptorSetLayouts.data();
    }
    createInfo.pushConstantRangeCount = uint32_t(pipeline.pushConstantRanges.size());
    if(createInfo.pushConstantRangeCount > 0) {
        createInfo.pPushConstantRanges = pipeline.pushConstantRanges.data();
    }

    Result result;
    if(VKFAILED(result = vkCreatePipelineLayout(*m_device, &createInfo, nullptr, &pipeline.pipelineLayout))) {
        qCCritical(logVulkan) << "PipelineBuilder: Failed to create pipeline layout" << result.toString();
        m_device->destroyPipeline(pipeline);
        return false;
    }

    return true;
}

QVector<VkDescriptorSetLayout> PipelineBuilder::buildDescriptorSetLayouts() const
{
    struct DescriptorSetLayoutBinding {
        VkDescriptorType type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        VkShaderStageFlags stageFlags = 0;
        uint32_t count = 0;
        VkDescriptorBindingFlagsEXT flags = 0;
        QVector<VkSampler> samplers;
    };
    using DescriptorSetLayout = QVector<DescriptorSetLayoutBinding>;
    QVector<DescriptorSetLayout> layouts;

    QMultiMap<QString, DescriptorSetLayoutBinding*> bindingsByName;

    for(const ShaderModule *shader : m_shaders) {
        for(const ShaderModule::DescriptorSetLayout &shaderSetLayout : shader->descriptorSets()) {
            int setNumber = int(shaderSetLayout.set);
            if(setNumber >= layouts.size()) {
                layouts.resize(setNumber+1);
            }

            DescriptorSetLayout &setLayout = layouts[setNumber];
            for(const ShaderModule::DescriptorSetLayoutBinding &shaderBinding : shaderSetLayout.bindings) {
                int bindingNumber = int(shaderBinding.binding);
                if(bindingNumber >= setLayout.size()) {
                    setLayout.resize(bindingNumber+1);
                }

                DescriptorSetLayoutBinding &binding = setLayout[bindingNumber];
                const auto &bindingInfo = m_descriptorBindingInfoByID.value(qMakePair(setNumber, bindingNumber));

                if(binding.type == VK_DESCRIPTOR_TYPE_MAX_ENUM) {
                    binding.type = shaderBinding.type;
                }
                if(binding.count == 0) {
                    binding.count = shaderBinding.count;
                }

                bool bindingConflict = false;
                if(binding.type != shaderBinding.type || (bindingInfo.count == 0 && binding.count != shaderBinding.count)) {
                    bindingConflict = true;
                }
                if(bindingConflict) {
                    qCWarning(logVulkan) << "PipelineBuilder: Conflicting descriptor set layout binding detected at (set ="
                                         << setNumber << ", binding =" << bindingNumber << ")";
                }
                else {
                    bindingsByName.insert(shaderBinding.name, &binding);

                    binding.stageFlags |= shader->stage() | bindingInfo.stageFlags;
                    binding.flags = bindingInfo.flags;
                    if(bindingInfo.count > 0) {
                        binding.count = bindingInfo.count;
                    }

                    if(binding.type == VK_DESCRIPTOR_TYPE_SAMPLER || binding.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                        VkSampler sampler = m_samplersByID.value(qMakePair(setNumber, bindingNumber), VK_NULL_HANDLE);
                        if(sampler != VK_NULL_HANDLE) {
                            binding.samplers.fill(sampler, int(binding.count));
                        }
                    }
                }
            }
        }
    }

    for(auto it = m_descriptorBindingInfoByName.begin(); it != m_descriptorBindingInfoByName.end(); ++it) {
        auto bindings = bindingsByName.values(it.key());
        if(bindings.size() == 0) {
            qCWarning(logVulkan) << "PipelineBuilder: No such descriptor set binding:" << it.key();
            continue;
        }
        const auto &bindingInfo = it.value();
        for(DescriptorSetLayoutBinding *binding : bindings) {
            if(bindingInfo.count > 0) {
                binding->count = bindingInfo.count;
            }
            binding->stageFlags |= bindingInfo.stageFlags;
            binding->flags |= bindingInfo.flags;
        }
    }

    for(auto it = m_samplersByName.begin(); it != m_samplersByName.end(); ++it) {
        auto bindings = bindingsByName.values(it.key());
        for(DescriptorSetLayoutBinding *binding : bindings) {
            if(binding->samplers.size() == 0) {
                binding->samplers.fill(it.value(), int(binding->count));
            }
            else {
                qCWarning(logVulkan) << "PipelineBuilder: Conflicting immutable sampler assignment detected for descriptor set binding variable name"
                                     << it.key();
            }
        }
    }

    if(m_defaultSampler != VK_NULL_HANDLE) {
        for(DescriptorSetLayout &setLayout : layouts) {
            for(DescriptorSetLayoutBinding &binding : setLayout) {
                if(binding.count > 0 && (binding.type == VK_DESCRIPTOR_TYPE_SAMPLER || binding.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)) {
                    binding.samplers.fill(m_defaultSampler, int(binding.count));
                }
            }
        }
    }

    QVector<VkDescriptorSetLayout> vulkanLayouts;
    for(int index=0; index < layouts.size(); ++index) {
        const DescriptorSetLayout &setLayout = layouts[index];

        QVector<VkDescriptorSetLayoutBinding> vulkanBindings;
        QVector<VkDescriptorBindingFlagsEXT> vulkanBindingFlags;

        VkDescriptorSetLayoutBindingFlagsCreateInfoEXT layoutBindingFlagsCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT };
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };

        layoutCreateInfo.pNext = &layoutBindingFlagsCreateInfo;
        layoutCreateInfo.flags = m_descriptorSetLayoutFlags.value(uint32_t(index), 0);

        if(setLayout.size() > 0) {
            for(uint32_t bindingNumber=0; bindingNumber < uint32_t(setLayout.size()); ++bindingNumber) {
                const DescriptorSetLayoutBinding &binding = setLayout[int(bindingNumber)];
                if(binding.count > 0) {
                    VkDescriptorSetLayoutBinding vulkanBinding = {};
                    vulkanBinding.binding = bindingNumber;
                    vulkanBinding.stageFlags = binding.stageFlags;
                    vulkanBinding.descriptorType = binding.type;
                    vulkanBinding.descriptorCount = binding.count;
                    if(binding.samplers.size() > 0) {
                        vulkanBinding.pImmutableSamplers = binding.samplers.data();
                    }
                    if(binding.flags & VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT) {
                        layoutCreateInfo.flags |= VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
                    }
                    vulkanBindings.append(vulkanBinding);
                    vulkanBindingFlags.append(binding.flags);
                }
            }
            if(vulkanBindings.size() > 0) {
                Q_ASSERT(vulkanBindingFlags.size() == vulkanBindings.size());
                layoutCreateInfo.bindingCount = uint32_t(vulkanBindings.size());
                layoutCreateInfo.pBindings = vulkanBindings.data();
                layoutBindingFlagsCreateInfo.bindingCount = uint32_t(vulkanBindingFlags.size());
                layoutBindingFlagsCreateInfo.pBindingFlags = vulkanBindingFlags.data();
            }
        }

        VkDescriptorSetLayout vulkanSetLayout;
        if(VKFAILED(vkCreateDescriptorSetLayout(*m_device, &layoutCreateInfo, nullptr, &vulkanSetLayout))) {
            qCCritical(logVulkan) << "PipelineBuilder: Failed to create descriptor set layout";
            break;
        }
        vulkanLayouts.append(vulkanSetLayout);
    }
    return vulkanLayouts;
}

QVector<VkPushConstantRange> PipelineBuilder::buildPushConstantRanges() const
{
    QMap<uint64_t, VkPushConstantRange> rangesMap;
    for(const ShaderModule *shader : m_shaders) {
        for(const auto &pushConstantRange : shader->pushConstants()) {
            const uint64_t hash = (uint64_t(pushConstantRange.size) << 32) | pushConstantRange.offset;
            auto it = rangesMap.find(hash);
            if(it == rangesMap.end()) {
                rangesMap.insert(hash, VkPushConstantRange{shader->stage(), pushConstantRange.offset, pushConstantRange.size});
            }
            else {
                auto &range = *it;
                range.stageFlags |= shader->stage();
            }
        }
    }

    QVector<VkPushConstantRange> ranges;
    ranges.reserve(rangesMap.size());
    for(const VkPushConstantRange &pushConstantRange : rangesMap) {
        ranges.append(pushConstantRange);
    }
    return ranges;
}

PipelineBuilder &PipelineBuilder::shaders(const QVector<const ShaderModule*> &modules)
{
    auto pipelineContainsShaderStage = [this](VkShaderStageFlagBits stage) -> bool {
        for(const ShaderModule *module : m_shaders) {
            if(module->stage() == stage) {
                return true;
            }
        }
        return false;
    };

    for(const ShaderModule *module : modules) {
        if(pipelineContainsShaderStage(module->stage())) {
            qCWarning(logVulkan) << "PipelineBuilder: pipeline already contains shader module for stage:" << module->stage();
        }
        else {
            m_shaders.append(module);
        }
    }
    return *this;
}

PipelineBuilder &PipelineBuilder::shaders(std::initializer_list<QString> moduleNames)
{
    QVector<const ShaderModule*> modules;
    modules.reserve(static_cast<int>(moduleNames.size()));
    for(const QString &name : moduleNames) {
        ShaderModule *module{new ShaderModule(*m_device, name)};
        modules.append(module);
        m_ownedModules.append(module);
    }
    return shaders(modules);
}

PipelineBuilder &PipelineBuilder::bytecodes(std::initializer_list<QByteArray> moduleBytecodes)
{
    QVector<const ShaderModule*> modules;
    modules.reserve(static_cast<int>(moduleBytecodes.size()));
    for(const QByteArray &bytecode : moduleBytecodes) {
        ShaderModule *module{new ShaderModule(*m_device, bytecode)};
        modules.append(module);
        m_ownedModules.append(module);
    }
    return shaders(modules);
}

PipelineBuilder &PipelineBuilder::defaultSampler(VkSampler sampler)
{
    m_defaultSampler = sampler;
    return *this;
}

PipelineBuilder &PipelineBuilder::sampler(uint32_t set, uint32_t binding, VkSampler sampler)
{
    m_samplersByID.insert(qMakePair(set, binding), sampler);
    return *this;
}

PipelineBuilder &PipelineBuilder::sampler(const QString &name, VkSampler sampler)
{
    m_samplersByName.insert(name, sampler);
    return *this;
}

PipelineBuilder &PipelineBuilder::descriptorSetLayoutFlags(uint32_t set, VkDescriptorSetLayoutCreateFlags flags)
{
    m_descriptorSetLayoutFlags.insert(set, flags);
    return *this;
}

PipelineBuilder &PipelineBuilder::descriptorBindingFlags(uint32_t set, uint32_t binding, VkDescriptorBindingFlagsEXT flags)
{
    m_descriptorBindingInfoByID[{set, binding}].flags = flags;
    return *this;
}

PipelineBuilder &PipelineBuilder::descriptorBindingFlags(const QString &name, VkDescriptorBindingFlagsEXT flags)
{
    m_descriptorBindingInfoByName[name].flags = flags;
    return *this;
}

PipelineBuilder &PipelineBuilder::descriptorBindingCount(uint32_t set, uint32_t binding, uint32_t count)
{
    m_descriptorBindingInfoByID[{set, binding}].count = count;
    return *this;
}

PipelineBuilder &PipelineBuilder::descriptorBindingCount(const QString &name, uint32_t count)
{
    m_descriptorBindingInfoByName[name].count = count;
    return *this;
}

PipelineBuilder &PipelineBuilder::descriptorBindingManager(uint32_t set, uint32_t binding, const DescriptorManager *manager, ResourceClass rclass)
{
    Q_ASSERT(manager);
    auto &info = m_descriptorBindingInfoByID[{set, binding}];
    info.count = manager->descriptorPoolCapacity(rclass);
    info.flags = manager->descriptorBindingFlags(rclass);
    info.stageFlags = VK_SHADER_STAGE_ALL;
    return *this;
}

PipelineBuilder &PipelineBuilder::descriptorBindingManager(const QString &name, const DescriptorManager *manager, ResourceClass rclass)
{
    Q_ASSERT(manager);
    auto &info = m_descriptorBindingInfoByName[name];
    info.count = manager->descriptorPoolCapacity(rclass);
    info.flags = manager->descriptorBindingFlags(rclass);
    info.stageFlags = VK_SHADER_STAGE_ALL;
    return *this;
}

} // Vulkan
} // Qt3DRaytrace
