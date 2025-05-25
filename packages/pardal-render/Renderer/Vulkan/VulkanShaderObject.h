
#pragma once
#include <Renderer/IShaderObject.h>
#include <vulkan/vulkan.hpp>

// Created on 2025-05-24 by sisco

namespace pdl
{

class VulkanShaderObject : public IShaderObject
{
public:
    VulkanShaderObject(ShaderObjectDescriptor& desc, vk::Device device);  
    ~VulkanShaderObject() override;

    vk::ShaderEXT GetVkShader() const { return m_shader; }
    ShaderType GetShaderType() const override { return m_shaderType; };
private:
    vk::ShaderEXT m_shader {};
    vk::Device m_device;
    ShaderType m_shaderType;
};

}

