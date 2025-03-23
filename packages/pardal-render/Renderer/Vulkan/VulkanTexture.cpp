
#include <Renderer/Vulkan/VulkanTexture.h>

// Created on 2025-03-23 by sisco

namespace pdl
{
    VulkanTexture::VulkanTexture(const Descriptor& desc, vk::Device* device)
    : m_vkDevice(device), m_descriptor(desc)
    {
        
    }

    VulkanTexture::~VulkanTexture()
    {
        if (m_ownsMemory)
        {
            Destroy();
        }
    }

    void VulkanTexture::Destroy()
    {
        m_vkDevice->freeMemory(m_vkDeviceMemory);
        m_vkDevice->destroyImage(m_vkImage, nullptr);
        m_vkImage = nullptr;
    }
}

