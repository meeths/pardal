
#pragma once
#include <Renderer/ITexture.h>
#include <vulkan/vulkan.hpp>
// Created on 2025-03-23 by sisco

namespace pdl
{

    class VulkanTexture : public ITexture
    {
    public:

        VulkanTexture(const Descriptor& desc, vk::Device* device);
        ~VulkanTexture() override;
    
        Descriptor* GetDescriptor() override { return &m_descriptor; }
        TextureType GetTextureType() const override { return TextureType::Texture2D; }
    
        vk::Image m_vkImage;
        vk::Format m_vkFormat = vk::Format::eR8G8B8A8Unorm;
        vk::DeviceMemory m_vkDeviceMemory;
        vk::Device* m_vkDevice;
    
        bool m_ownsMemory = true;
    protected:
        void Destroy();
        Descriptor m_descriptor;
    };

}

