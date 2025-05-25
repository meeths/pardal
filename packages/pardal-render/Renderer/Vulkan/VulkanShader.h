
#pragma once
#include "Renderer/IShader.h"
#include <vulkan/vulkan.hpp>
// Created on 2025-05-24 by sisco

namespace pdl
{

class VulkanShader : public IShader
{
public:
    VulkanShader(const ShaderDescriptor& descriptor);
    ~VulkanShader() override;

    void Bind(ICommandBuffer* commandBuffer) override;
private:
    Vector<vk::ShaderEXT> m_shaderObjects;
    Vector<vk::ShaderStageFlagBits> m_shaderStages;
};

}

