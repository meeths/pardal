
#pragma once
#include <Renderer/ITexture.h>
#include <vulkan/vulkan.hpp>
#include <Base/BaseDefines.h>
// Created on 2025-03-23 by sisco

namespace pdl
{

    class VulkanTexture : public ITexture
    {
    public:

        VulkanTexture(const TextureDescriptor& desc, vk::Device* device);
        ~VulkanTexture() override;

        DeclareNonCopyable(VulkanTexture);
        
        bool Initialize(const vk::PhysicalDevice* physicalDevice);
        bool Initialize(vk::Image image);
        
        const TextureDescriptor& GetDescriptor() const override { return m_textureDescriptor; }
        TextureType GetTextureType() const override { return TextureType::Texture2D; }

        const vk::Image& GetVkImage() const { return m_vkImage; }
        const vk::Format& GetVkFormat() const { return m_vkFormat; }
    protected:
        void Destroy();
        TextureDescriptor m_textureDescriptor;

        vk::Image m_vkImage {};
        vk::Format m_vkFormat = vk::Format::eR8G8B8A8Unorm;
        vk::DeviceMemory m_vkDeviceMemory {};
        vk::Device* m_vkDevice;
    
        bool m_ownsMemory = true;
    };

}

