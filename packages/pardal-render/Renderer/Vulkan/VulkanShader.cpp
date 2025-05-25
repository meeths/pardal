
#include <Renderer/Vulkan/VulkanShader.h>

#include <Renderer/Vulkan/VulkanCommandBuffer.h>
#include <Renderer/Vulkan/VulkanShaderObject.h>
#include <Renderer/Vulkan/VulkanUtils.h>

// Created on 2025-05-24 by sisco

namespace pdl
{
    VulkanShader::VulkanShader(const ShaderDescriptor& descriptor)
    {
        for (auto shader : descriptor.m_shaderObjects)
        {
            m_shaderObjects.push_back(static_cast<VulkanShaderObject*>(shader)->GetVkShader());
            m_shaderStages.push_back(VulkanUtils::GetShaderStageBits(shader->GetShaderType()));
        }
    }

    VulkanShader::~VulkanShader()
    {
    }

    void VulkanShader::Bind(ICommandBuffer* commandBuffer)
    {
        pdlAssert(m_shaderObjects.size() == m_shaderStages.size() && !m_shaderObjects.empty());
        
        vk::CommandBuffer vkCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer)->GetVkCommandBuffer();
        vkCommandBuffer.bindShadersEXT(m_shaderObjects.size(), m_shaderStages.data(), m_shaderObjects.data());
            
        vkCommandBuffer.draw(3, 1, 0, 0);
    }
}

