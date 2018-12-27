/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/shadermodule.h>
#include <spirv_reflect.h>

#include <QFile>
#include <QDebug>
#include <QVector>

namespace Qt3DRaytrace {
namespace Vulkan {

ShaderModule::ShaderModule(VkDevice device, const QString &filename)
{
    loadFromFile(device, filename);
}

ShaderModule::ShaderModule(VkDevice device, const QByteArray &bytecode)
{
    loadFromBytecode(device, bytecode);
}

ShaderModule::~ShaderModule()
{
    release();
}

bool ShaderModule::loadFromFile(VkDevice device, const QString &filename)
{
    QFile moduleFile(filename);
    if(!moduleFile.open(QFile::ReadOnly)) {
        qCCritical(logVulkan) << "Cannot open shader module file:" << filename;
        return false;
    }

    const QByteArray bytecode = moduleFile.readAll();
    if(bytecode.size() == 0) {
        qCCritical(logVulkan) << "Failed to read SPIR-V bytecode from shader module file:" << filename;
        return false;
    }

    return loadFromBytecode(device, bytecode);
}

bool ShaderModule::loadFromBytecode(VkDevice device, const QByteArray &bytecode)
{
    Q_ASSERT(device != VK_NULL_HANDLE);
    release();
    m_device = device;

    VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    createInfo.codeSize = static_cast<unsigned long>(bytecode.size());
    createInfo.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());
    if(VKFAILED(vkCreateShaderModule(m_device, &createInfo, nullptr, &m_handle))) {
        qCCritical(logVulkan) << "Failed to create Vulkan shader module";
        return false;
    }

    spv_reflect::ShaderModule spvModule(static_cast<size_t>(bytecode.size()), bytecode.data());
    m_stage = static_cast<VkShaderStageFlagBits>(spvModule.GetShaderStage());

    uint32_t descriptorSetCount = 0;
    spvModule.EnumerateDescriptorSets(&descriptorSetCount, nullptr);
    if(descriptorSetCount > 0) {
        QVector<SpvReflectDescriptorSet*> descriptorSets(static_cast<int>(descriptorSetCount));
        spvModule.EnumerateDescriptorSets(&descriptorSetCount, descriptorSets.data());
        m_descriptorSets.reserve(static_cast<int>(descriptorSetCount));
        for(const SpvReflectDescriptorSet *spvReflectDescriptorSet : descriptorSets) {
            DescriptorSetLayout descriptorSet = { spvReflectDescriptorSet->set };
            descriptorSet.bindings.reserve(static_cast<int>(spvReflectDescriptorSet->binding_count));
            for(unsigned int i=0; i<spvReflectDescriptorSet->binding_count; ++i) {
                const SpvReflectDescriptorBinding *spvReflectBinding = spvReflectDescriptorSet->bindings[i];
                DescriptorSetLayoutBinding binding;
                binding.name = QString(spvReflectBinding->name);
                binding.type = static_cast<VkDescriptorType>(spvReflectBinding->descriptor_type);
                binding.count = spvReflectBinding->count;
                descriptorSet.bindings.append(binding);
            }
            m_descriptorSets.append(descriptorSet);
        }
    }

    uint32_t pushConstantBlockCount = 0;
    spvModule.EnumeratePushConstantBlocks(&pushConstantBlockCount, nullptr);
    if(pushConstantBlockCount > 0) {
        QVector<SpvReflectBlockVariable*> pushConstantBlocks(static_cast<int>(pushConstantBlockCount));
        spvModule.EnumeratePushConstantBlocks(&pushConstantBlockCount, pushConstantBlocks.data());
        m_pushConstants.reserve(static_cast<int>(pushConstantBlockCount));
        for(const SpvReflectBlockVariable *spvReflectPushConstantBlock : pushConstantBlocks) {
            PushConstantRange range;
            range.size = spvReflectPushConstantBlock->size;
            range.offset = spvReflectPushConstantBlock->offset;
            m_pushConstants.append(range);
        }
    }

    return true;
}

void ShaderModule::release()
{
    if(isValid()) {
        vkDestroyShaderModule(m_device, m_handle, nullptr);
        m_descriptorSets.clear();
        m_pushConstants.clear();
    }
    reset();
}

} // Vulkan
} // Qt3DRaytrace
