/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <renderers/vulkan/shadermodule.h>
#include <renderers/vulkan/device.h>
#include <spirv_reflect.h>

#include <QFile>
#include <QDebug>
#include <QVector>

namespace Qt3DRaytrace {
namespace Vulkan {

ShaderModule::ShaderModule(VkDevice device, const QString &filename, const QString &entryPoint)
{
    loadFromFile(device, filename, entryPoint);
}

ShaderModule::ShaderModule(VkDevice device, const QByteArray &bytecode, const QString &entryPoint)
{
    loadFromBytecode(device, bytecode, entryPoint);
}

ShaderModule::~ShaderModule()
{
    destroy();
}

bool ShaderModule::loadFromFile(VkDevice device, const QString &name, const QString &entryPoint)
{
    const QString path = getModulePath(name);

    QFile moduleFile(path);
    if(!moduleFile.open(QFile::ReadOnly)) {
        qCCritical(logVulkan) << "Cannot open shader module file:" << path;
        return false;
    }

    const QByteArray bytecode = moduleFile.readAll();
    if(bytecode.size() == 0) {
        qCCritical(logVulkan) << "Failed to read SPIR-V bytecode from shader module file:" << path;
        return false;
    }

    return loadFromBytecode(device, bytecode, entryPoint);
}

bool ShaderModule::loadFromBytecode(VkDevice device, const QByteArray &bytecode, const QString &entryPoint)
{
    Q_ASSERT(device);

    destroy();

    m_device = device;
    m_entryPoint = entryPoint.toLocal8Bit();

    VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    createInfo.codeSize = static_cast<unsigned long>(bytecode.size());
    createInfo.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());
    if(VKFAILED(vkCreateShaderModule(m_device, &createInfo, nullptr, &m_module))) {
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
                binding.binding = spvReflectBinding->binding;
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
            range.name = QString(spvReflectPushConstantBlock->name);
            range.size = spvReflectPushConstantBlock->size;
            range.offset = spvReflectPushConstantBlock->offset;
            m_pushConstants.append(range);
        }
    }

    return true;
}

void ShaderModule::destroy()
{
    if(isValid()) {
        vkDestroyShaderModule(m_device, m_module, nullptr);
        m_descriptorSets.clear();
        m_pushConstants.clear();
        m_module = VK_NULL_HANDLE;
    }
}

QString ShaderModule::getModulePath(const QString &name)
{
    // TODO: Move into application directory or (even better) embed SPIR-V modules into DLL/so via Qt resource system.
    return QString("%1/%2.spv").arg("./shaders").arg(name);
}

} // Vulkan
} // Qt3DRaytrace
