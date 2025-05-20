
#pragma once
#include <Renderer/ITextureView.h>
#include <vulkan/vulkan.hpp>

#include "Base/BaseDefines.h"

// Created on 2025-03-26 by sisco

namespace pdl
{

class VulkanTextureView : public ITextureView
{
public:
    VulkanTextureView(const TextureViewDescriptor& desc, vk::Device* device);
    ~VulkanTextureView() override;

    DeclareNonCopyable(VulkanTextureView);
    
    ITexture* GetTexture() const { return m_descriptor.m_texture; }
    
    vk::ImageView GetVkImageView() const { return m_imageView; }
    vk::Sampler GetVkSampler() const { return m_sampler; }
private:
    bool Initialize();
    TextureViewDescriptor m_descriptor;
    vk::Device* m_device;
    vk::ImageView m_imageView = VK_NULL_HANDLE;
    vk::Sampler m_sampler = VK_NULL_HANDLE;

    static vk::Sampler m_defaultSampler;
};

}

