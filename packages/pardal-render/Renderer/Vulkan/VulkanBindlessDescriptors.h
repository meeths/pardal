
#pragma once

// Created on 2025-05-04 by sisco
#include <vulkan/vulkan.hpp>
#include <Renderer/BindlessDescriptors.h>


namespace pdl
{
class VulkanDevice;

class VulkanBindlessDescriptors : public BindlessDescriptors
{
public:
    VulkanBindlessDescriptors(const VulkanDevice& _device);
    ~VulkanBindlessDescriptors() override = default;
    
    bool Initialize(BindlessDescriptorInfo& info) override;
    
    TextureDescriptorHandle StoreTexture(ITextureView* textureView) override;
    BufferDescriptorHandle StoreBuffer(IRenderBuffer* buffer) override;

    void WriteDescriptors() override;

    bool Bind(ICommandBuffer* commandBuffer) override;
private:
    vk::Device m_device;
    vk::DescriptorSetLayout m_bindlessLayout;
    vk::DescriptorPool m_bindlessPool;
    vk::DescriptorSet m_bindlessSet;

    Vector<ITextureView*> m_textureViews;
    Vector<IRenderBuffer*> m_buffers;
    Vector<TextureDescriptorHandle> m_freeTextureHandles;
    Vector<BufferDescriptorHandle> m_freeBufferHandles;

    Vector<vk::WriteDescriptorSet> m_pendingWrites;
    
    static constexpr uint32 kInvalidBindingIndex = static_cast<uint32>(-1);
    uint32 m_textureBindingIndex = kInvalidBindingIndex;
    uint32 m_uniformBindingIndex = kInvalidBindingIndex;
    uint32 m_storageBindingIndex = kInvalidBindingIndex;
    uint32 m_accelerationStructureBindingIndex = kInvalidBindingIndex;
};

}

