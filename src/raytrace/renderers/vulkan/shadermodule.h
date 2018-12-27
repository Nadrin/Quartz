/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>

#include <QString>
#include <QByteArray>
#include <QVector>

namespace Qt3DRaytrace {
namespace Vulkan {

class ShaderModule : public DeviceHandle<VkShaderModule>
{
public:
    struct DescriptorSetLayoutBinding {
        QString name;
        VkDescriptorType type;
        uint32_t count;
    };
    struct DescriptorSetLayout {
        uint32_t set;
        QVector<DescriptorSetLayoutBinding> bindings;
    };
    struct PushConstantRange {
        uint32_t offset;
        uint32_t size;
    };

    ShaderModule() = default;
    ShaderModule(VkDevice device, const QString &filename);
    ShaderModule(VkDevice device, const QByteArray &bytecode);
    ~ShaderModule();

    bool loadFromFile(VkDevice device, const QString &filename);
    bool loadFromBytecode(VkDevice device, const QByteArray &bytecode);
    void release();

    VkShaderStageFlagBits stage() const { return m_stage; }
    const QVector<DescriptorSetLayout> &descriptorSets() const { return m_descriptorSets; }
    const QVector<PushConstantRange> &pushConstants() const { return m_pushConstants; }

private:
    VkShaderStageFlagBits m_stage;
    QVector<DescriptorSetLayout> m_descriptorSets;
    QVector<PushConstantRange> m_pushConstants;
};

} // Vulkan
} // Qt3DRaytrace
